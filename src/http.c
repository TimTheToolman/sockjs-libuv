#include <assert.h>
#include <stdlib.h>
#include "http.h"
#include "memory.h"

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

	memory_release_buf(req->body);
}

// Response stuff
void http_response_init(http_response_t* resp)
{
	memset(resp, 0, sizeof(http_response_t));
}

void http_response_free(http_response_t* resp)
{
	memory_release_buf(resp->headers);
	memory_release_buf(resp->body);
	memory_release_buf(resp->response[0]);
}

void http_response_add_header(http_response_t* resp, const char* header, size_t length)
{
	int len = resp->headers.len;

	if (len == 0)
		len = 64;

	size_t old_len = resp->headers_len;
	size_t new_len = old_len + length + 1;

	while (new_len > len)
		len *= 2;

	if (resp->headers.len != len) {
		resp->headers = memory_realloc(resp->headers, len);
	}

	char* buf = (char*)resp->headers.base;
	memcpy(buf + old_len, header, length);
	buf[new_len] = '\0';

	resp->headers_len = new_len - 1;
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
