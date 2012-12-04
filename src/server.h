#ifndef SERVER_H_
#define SERVER_H_

#include <uv.h>

// HTTP server state
typedef struct {
	uv_tcp_t tcp;
	uv_loop_t* loop;
} server_t;

#endif
