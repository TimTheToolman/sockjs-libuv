#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include "server.h"
#include "client.h"

// Server
void server_on_connection(uv_stream_t* stream, int status) {
	// Initialize client structure
	server_t* server = (server_t*)stream->data;

	if (status == -1) {
		ERROR(server->loop);
		return;
	}

	client_t* client = (client_t*)malloc(sizeof(client_t));
	client_init(server, client);
}

server_t* server_init(uv_loop_t* loop, const char* addr, int port) {
	server_t* server = (server_t*)malloc(sizeof(server_t));
	server->loop = loop;

	// Initialize TCP server
	if (uv_tcp_init(loop, &server->tcp)) {
		ERROR(loop)
		free(server);
		return NULL;
	}
	server->tcp.data = server;

	// TODO: Fix me
	struct sockaddr_in address = uv_ip4_addr(addr, port);

	if (uv_tcp_bind(&server->tcp, address)) {
		ERROR(loop);
		uv_close((uv_handle_t*)&server->tcp, NULL);
		free(server);
		return NULL;
	}

	if (uv_listen((uv_stream_t*)&server->tcp, 128, server_on_connection)) {
		ERROR(loop);
		uv_close((uv_handle_t*)&server->tcp, NULL);
		free(server);
		return NULL;
	}

	return server;
}

int main(int argc, char** argv) {
	uv_loop_t* loop = uv_default_loop();
	server_init(loop);
	uv_run(loop);
	return 0;
}
