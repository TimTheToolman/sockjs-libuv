#include "global.h"
#include "router.h"


int check_path(const char* path, const char* needle, int len) {
	return memcmp(path, needle, len);
}

#define VERIFY_PATH(path, n)	check_path(path + 1, n, sizeof(n) - 1)

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
				if (VERIFY_PATH(c, "ello")) {
					handle_hello(client);
					return;
				}

				break;
		}
	}

	http_response_set_error(client->response, 404);
    client_finish_response(client);
}
