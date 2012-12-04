#include <uv.h>
#include "router.h"
#include "memory.h"


int check_path(const char* path, const char* needle, int len) {
	return memcmp(path, needle, len);
}

#define CHECK(path, n)	check_path(path + 1, n, sizeof(n) - 1)

int handle_hello(client_t* client) {
	client->response.status_code = 200;
	client->response.body = memory_alloc(sizeof(11));
	memcpy(client->body.base, "Hello World", 11);
	client_send_response(client);
}

void route_request(client_t* client)
{
	http_request_t* request = client->request;

	const char* url = request->url;

	// TODO: Check prefix
	len = strlen(url);

	if (len < 2) {
		// TODO: Logging
		return -1;
	}

	const char* c = url;

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