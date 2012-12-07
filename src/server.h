#ifndef SERVER_H_
#define SERVER_H_

// HTTP server state
typedef struct {
	uv_tcp_t stream;
	uv_loop_t* loop;
} server_t;

#endif
