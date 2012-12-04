#ifndef CLIENT_H_
#define CLIENT_H_

#include "server.h"
#include "http.h"
#include "queue.h"

typedef struct {
	// Server reference
	server_t* server;

	// Connection related
	uv_tcp_t tcp;
	http_parser parser;

	int is_active;

	// Request states
	http_request_t* request;
	queue_t request_queue;

	http_request_t* parsing;

	// Response
	http_response_t* response;
} http_client_t;

void client_init(http_client_t* client);
void client_send_response(http_client_t* client);

#endif