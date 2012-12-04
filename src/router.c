#include <string.h>
#include <stdlib.h>
#include "router.h"
#include "memory.h"


int check_path(const char* path, const char* needle, int len) {
	return memcmp(path, needle, len);
}

#define CHECK(path, n)	check_path(path + 1, n, sizeof(n) - 1)

void handle_hello(client_t* client) {
	client->response->status_code = 200;
	client->response->body = uv_buf_init(malloc(sizeof(11)), 11);
	memcpy(client->response->body.base, "Hello World", 11);
	client_send_response(client);
}

void route_request(client_t* client)
{
	http_request_t* request = client->request;

	const char* c = request->url;

	if (*(c++) == '/')
	{
		switch (*(c++)) {
			case 'h':
				if (CHECK(c, "ello")) {
					handle_hello(client);
					return;
				}

				break;
		}
	}

	http_response_set_error(client->response, 404);
}
