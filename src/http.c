#include "global.h"

#define HEADER "%s: %s\r\n"

// Request stuff
void http_request_init(http_request_t* req)
{
	memset(req, 0, sizeof(http_request_t));
}

void http_request_free(http_request_t* req)
{
	if (req->url != NULL) {
		free(req->url);
		req->url = NULL;
	}

	str_free(&req->body);
}

// Response stuff
void http_response_init(http_response_t* resp)
{
	memset(resp, 0, sizeof(http_response_t));
}

void http_response_free(http_response_t* resp)
{
	str_free(&resp->headers);
	str_free(&resp->body);
	str_free(&resp->response_buf);
}

void http_response_set_error(http_response_t* resp, int status_code)
{
	resp->status_code = status_code;
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
			return "Internal Server Error";
		default:
			assert(FALSE);
			return "";
	}
}
