#ifndef	HTTP_H_
#define HTTP_H_

// HTTP request
typedef struct {
	char* method;
	char* url;
	str_t body;

	// Header data
	int http_version_minor;
	int keep_alive;
} http_request_t;

// HTTP response
typedef struct {
	// Output status code
	int status_code;
	// HTTP protocol version (1.x)
	int http_version_minor;
	// Output header blob
	str_t headers;

    // TODO: Special bool type
    int headers_sent;

	// Response body
	str_t body;

	// Output headers and HTTP code
	str_t response_buf;

	uv_write_t hdr_write;
	uv_write_t body_write;

	int keep_alive;
} http_response_t;

// API
void http_request_init(http_request_t* req);
void http_request_free(http_request_t* req);

void http_response_init(http_response_t* resp);
void http_response_free(http_response_t* resp);

#define http_response_add_header(resp, header, len)	\
	str_append(&resp->headers, header, len);

void http_response_set_error(http_response_t* resp, int status_code);

const char* http_get_status_string(int status_code);

#endif
