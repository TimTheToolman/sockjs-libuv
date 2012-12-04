#ifndef CLIENT_H_
#define CLIENT_H_

#include <uv.h>
#include <http_parser.h>
#include "server.h"
#include "http.h"
#include "queue.h"

struct client_s;

typedef void (* handler_t)(struct client_s* client, size_t nread, uv_buf_t buf);

typedef struct client_s {
	// Server reference
	server_t* server;

	// Connection related
	uv_tcp_t stream;
	http_parser parser;

	int is_active;

	// Handlers
	handler_t handler;

	// Request states
	http_request_t* request;
	queue_t request_queue;

	http_request_t* parsing;

	// Response
	http_response_t* response;
} client_t;

int client_init(server_t* server, client_t* client);
void client_send_headers(client_t* client);
void client_finish_response(client_t* client);

#endif
