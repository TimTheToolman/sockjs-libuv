#include "global.h"
#include "client.h"
#include "router.h"

#define HTTP_TEMPLATE "HTTP/1.%d %d %s\r\n"
#define HTTP_TAIL "\r\n"

// Request handling
void handle_request(client_t* client) {
	client->response = (http_response_t*)malloc(sizeof(http_response_t));
	http_response_init(client->response);

	client->response->keep_alive = client->request->keep_alive;
	client->response->http_version_minor = client->request->http_version_minor;

	route_request(client);
}

// HTTP parser callbacks
int on_message_begin(http_parser* p) {
	client_t* client = (client_t*)p->data;

	// Create new request state object
	http_request_t* request = malloc(sizeof(http_request_t));
	http_request_init(request);
	client->parsing = request;

	return 0;
}

int on_url(http_parser* p, const char* data, size_t len) {
	client_t* client = (client_t*)p->data;

	client->parsing->url = strndup(data, len);
	return 0;
}

int on_body(http_parser* p, const char* data, size_t len) {
	client_t* client = (client_t*)p->data;

	return 0;
}

int on_headers_complete(http_parser* p) {
	client_t* client = (client_t*)p->data;

	http_request_t* request = client->parsing;
	request->keep_alive = http_should_keep_alive(p);

	return 0;
}

int on_message_complete(http_parser* p) {
	client_t* client = (client_t*)p->data;

	// Support request pipelining
	if (client->request == NULL) {
		client->request = client->parsing;
		handle_request(client);
	} else
	{
		queue_add(&client->request_queue, client->parsing);
	}

	client->parsing = NULL;

	return 0;
}

http_parser_settings client_parser = {
	.on_message_begin = on_message_begin,
	.on_url = on_url,
	.on_body = on_body,
	.on_message_complete = on_message_complete
};

// Cleanup
void client_free(client_t* client) {
	if (client->parsing != NULL) {
		http_request_free(client->parsing);
		free(client->parsing);
        client->parsing = NULL;
	}

	if (client->request != NULL) {
		http_request_free(client->request);
		free(client->request);
        client->request = NULL;
	}

	if (client->response != NULL) {
		http_response_free(client->response);
		free(client->response);
        client->response = NULL;
	}

	// Reset request queue
	http_request_t* req;
	while ((req = (http_request_t*)queue_pop(&client->request_queue))) {
		http_request_free(req);
		free(req);
	}

	free(client);
}

void response_free(client_t* client, http_response_t* resp) {
	http_response_free(resp);
	free(resp);

	if (resp == client->response)
		client->response = NULL;
}

void after_close(uv_handle_t* handle) {
	client_t* client = (client_t*)handle->data;

	client_free(client);
}

void client_close(client_t* client) {
	if (client->is_active) {
		uv_close((uv_handle_t*)&client->stream, after_close);
		client->is_active = FALSE;
	}
}

void after_write(uv_write_t* req, int status) {
	client_t* client = (client_t*)req->handle->data;
	http_response_t* resp = (http_response_t*)req->data;

	if (status != 0) {
		ERROR(client->server->loop);
		client_close(client);
    } else
	if (!resp->keep_alive) {
		client_close(client);
	}

	response_free(client, resp);
}

void after_header_write(uv_write_t* req, int status) {
	client_t* client = (client_t*)req->handle->data;
	http_response_t* resp = (http_response_t*)req->data;

	if (status != 0) {
		ERROR(client->server->loop);
		client_close(client);
	}
	
	
}

// Network
void on_net_data(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
	client_t* client = (client_t*)stream->data;

	if (nread >= 0) {
		if (client->handler == NULL) {
			ssize_t parsed = http_parser_execute(&client->parser, &client_parser, buf.base, nread);

			if (parsed < nread) {
                client_close(client);
			}
		} else {
			// Pass buffer to the sub protocol handler
			client->handler(client, nread, buf);
		}
	} else {
        client_close(client);
	}

	memory_uv_free(buf);
}

// Client state
int client_init(server_t* server, client_t* client) {
	memset(client, 0, sizeof(client_t));

	client->server = server;
	client->is_active = TRUE;

	queue_init(&client->request_queue);

	uv_tcp_init(server->loop, &client->stream);
	client->stream.data = client;

	http_parser_init(&client->parser, HTTP_REQUEST);
	client->parser.data = client;

	if (uv_accept((uv_stream_t*)&server->stream, (uv_stream_t*)&client->stream) == 0) {
		uv_read_start((uv_stream_t*)&client->stream, memory_uv_alloc, on_net_data);
		return 0;
	} else {
		uv_close((uv_handle_t*)&client->stream, NULL);
		return -1;
	}
}

void client_send_headers(client_t* client)
{

}

void client_finish_response(client_t* client)
{
	http_response_t* resp = client->response;

}
