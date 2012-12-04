#include <assert.h>
#include "http.h"
#include "memory.h"

// Request stuff
void http_request_init(http_request_t* req)
{
	memset(req, 0, sizeof(http_request_t));
}

void http_request_free(http_request_t* req)
{
	if (req.url != NULL) {
		free(req.url);
		req.url = NULL;
	}

	if (req.body.base != NULL) {
		free(req.body);
		req.body.base = NULL;
		req.body.len = 0;
	}
}

// Response stuff
void http_response_init(http_response_t* resp)
{
	memset(resp, 0, sizeof(http_response_t));
}

void http_response_free(http_response_t* resp)
{
	if (resp->headers != NULL) {
		free(resp->headers);
		resp->headers = NULL;
	}

	resp->headers = memory_free(resp->headers);
	resp->body = memory_free(resp->body);
}

// Utilities
const char* http_get_status_string(int status_code)
{
	switch (status_code) {
		case 200:
			return "OK";
		case 400:
			return "Bad Request";
		case 404:
			return "Not Found";
		case 500:
			return "Internal Server Error"
		default:
			assert(FALSE);
			return ""
	}
}
