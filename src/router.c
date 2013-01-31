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


#define HEADER "Content-Type: text/html\r\n"


void route_request(client_t* client)
{
	http_request_t* request = client->request;

	const char* c = request->url;

						
	http_response_set_error(client->response, 404);
    client_finish_response(client);
}
