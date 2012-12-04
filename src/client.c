#include <uv.h>
#include <http_parser.h>
#include "client.h"
#include "memory.h"

#define HTTP_HEADER_TEMPLATE "HTTP/%d.%d %d %s\r\n%s\r\n"
#define HTTP_TEMPLATE "HTTP/%d.%d %d %s\r\n\r\n"

// Request handling
void handle_request(http_client_t* client) {
	client->response = (http_response_t*)malloc(sizeof(http_response_t));
	http_response_init(&client->response);

	client->response.keep_alive = client->request.keep_alive;
	client->response.http_version_minor = client->request.http_version_minor;

	// TODO: Handle request

	client_finish_request(client);
}

// HTTP parser callbacks
int on_message_begin(http_parser* p) {
	client_t* client = (client_t*)p->data;

	// Create new request state object
	request_state_t* state = malloc(sizeof(request_state_t));
	memset(state, 0, sizeof(request_state_t));
	state->client = client;

	client->state = request;

	return 0;
}

int on_url(http_parser* p, const char* data, size_t len) {
	client_t* client = (client_t*)p->data;

	client->state.request.url = strndup(data, len);
	return 0;
}

int on_body(http_parser* p, const char* data, size_t len) {
	client_t* client = (client_t*)p->data;

	// TODO: Body length validation
	http_request_t* request = &client->state->request;

	size_t old_len = request->body.len;
	if (old_len > 0) {
		request->body.base = realloc(request->body.base, old_len + len);
		request->body.len += len;
	} else {
		request->body.base = malloc(len);
		request->body.len = len;
	}

	memcpy(request->body.base + old_len, buf, len);

	return 0;
}

int on_headers_complete(http_parser* p) {
	client_t* client = (client_t*)p->data;

	http_request_t* request = &client->state->request;
	request->keep_alive = http_should_keep_alive(p);

	return 0;
}

int on_message_complete(http_parser* p) {
	client_t* client = (client_t*)p->data;

	// Support request pipelining
	if (client->active_state == NULL) {
		client->active_state = client->state;

		handle_request(client);
	} else
	{
		queue_add(client->state_queue, client->state);
	}

	client->state = NULL;

	return 0;
}

http_parser client_parser = {
	.on_message_begin = on_message_begin,
	.on_url = on_url,
	.on_body = on_body,
	.on_message_complete = on_message_complete
};

// Cleanup
void client_free(client_t* client) {
	if (client->state != NULL)
		free(client->state);

	if (client->active_state != NULL)
		free(client->active_state);

	// Reset request queue
	void* data;
	while (data = queue_pop(client->state_queue)) {
		free(data);
	}

	free(client);
}

void after_close(uv_handle_t* handle) {
	client_t* client = (client_t*)handle->data;

	client_free(client);
}

void client_close(client_t* client) {
	if (client->is_active) {
		uv_close((uv_handle_t*)&client->tcp, after_close);
		client->is_active = FALSE;
	}
}

void after_write(uv_write_t* req, int status) {
	client_t* client = (client_t*)req->handle;
	http_response_t* resp = (http_response_t*)req->data;

	if (status != 0) {
		ERROR(client->server->loop);
		client_close(client);
	}

	if (!resp->keep_alive)
		client_close(client);

	http_response_free(resp);
	free(resp);
}

// Network
void on_net_data(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
	client_t* client = (client_t*)stream->data;

	if (nread >= 0) {
		if (client->handler == NULL) {
			ssize_t parsed = http_parser_execute(&client->parser, &client_parser, buf.base, nread);

			if (parsed < nread) {
				uv_close((uv_handle_t *)&client->tcp, client_on_close);
			}
		} else {
			// Pass buffer to the sub protocol handler
			client->handler(client, nread, buf);
		}
	} else {
		uv_close((uv_handle_t *)&client->tcp, client_on_close);
	}

	free(buf.base);
}

// Client state
int client_init(server_t* server, client_t* client) {
	memset(client, 0, sizeof(client_t));

	client->server = server;
	client->is_active = TRUE;

	queue_init(&client->request_queue);

	uv_tcp_init(server->loop, &client->tcp);
	client->tcp.data = client;

	http_parser_init(&client->parser, HTTP_REQUEST);
	client->parser.data = client;

	if (uv_accept(stream, (uv_stream_t*)&client->tcp) == 0) {
		uv_read_start((uv_stream_t*)&client->tcp, memory_alloc, on_net_data);
		return 0;
	} else {
		uv_close((uv_handle_t*)&client->tcp, NULL);
		return -1;
	}
}

void client_send_response(http_client_t* client) {
	http_response_t* resp = client->response;

	// Generate HTTP response
	const char* status_string = http_get_status_string(resp->status_code);

	int response_len;

	if (resp->headers.len > 0)
	{
		response_len = snprintf(NULL, 0, HTTP_HEADER_TEMPLATE,
									resp->http_version_minor,
									resp->status_code,
									status_string,
									resp->headers.base);

		resp->response_buf = memory_alloc(response_len);
		snprintf(resp->response_buf.base, response_len, HTTP_HEADER_TEMPLATE,
									resp->http_version_minor,
									resp->status_code,
									status_string,
									resp->headers.base);
	} else {
		response_len = snprintf(NULL, 0, HTTP_TEMPLATE,
									resp->http_version_minor,
									resp->status_code,
									status_string);

		resp->response_buf = memory_alloc(response_len);
		snprintf(resp->response_buf.base, response_len, HTTP_TEMPLATE,
									resp->http_version_minor,
									resp->status_code,
									status_string);
	}

	// Prepare write request
	uv_buf_t* buf[2];
	buf[0] = &resp->headers;
	buf[1] = &resp->body;

	resp->write_req.data = resp;
	uv_write(&resp->write_req,
			(uv_stream_t*)client->tcp,
			buf,
			resp->body.len > 0 ? 2 : 1,
			on_write);

	// Cleanup previous request
	http_request_free(client->request);
	free(client->request);
	client->request = NULL;

	if (resp->keep_alive) {
		if (!queue_is_empty(client->request_queue)) {
			client->request = (http_request_t*)queue_pop(client->request_queue);
			handle_request(client);
		}
	}
}
