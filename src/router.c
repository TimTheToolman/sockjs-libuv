#include "global.h"
#include "router.h"


int partial_match(const char* path, const char* needle, size_t len) {
	return memcmp(path, needle, len) == 0;
}

int exact_match(const char* path, const char* needle, size_t len) {
	if (memcmp(path, needle, len) == 0) {
		char c = path[len];
		if (c == '\0' || c == '?')
			return TRUE;
	}

	return FALSE;
}

#define PARTIAL(path, n)	partial_match(path, n, sizeof(n) - 1)
#define EXACT(path, n)	exact_path(path, n, sizeof(n) - 1)

#define HEADER "Content-Type: text/html\r\n"

void handle_hello(client_t* client) {
	client->response->status_code = 200;

	// Set body
	str_append_len(&client->response->body, "Hello World", 11);

	// Set header
	str_append_len(&client->response->headers, HEADER, sizeof(HEADER) - 1);
	client_finish_response(client);
}

void route_request(client_t* client)
{
	http_request_t* request = client->request;

	const char* c = request->url;

	if (*(c++) == '/')
	{
		switch (*(c++)) {
			case 'h':
				if (EXACT(c, "ello")) {
					handle_hello(client);
					return;
				}
				break;
		}
	}

	http_response_set_error(client->response, 404);
    client_finish_response(client);
}
