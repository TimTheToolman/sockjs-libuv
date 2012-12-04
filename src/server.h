#ifndef SERVER_H_
#define SERVER_H_

#define ERROR(uv_loop) \
    uv_err_t err = uv_last_error(uv_loop); \
    fprintf(stderr, "%s:%d:%s() %s\n", __FILE__, __LINE__, __FUNCTION__, uv_strerror(err));

#define CHECK(action, ret) if (action) { ERROR() return ret; }

// HTTP server state
typedef struct {
	uv_tcp_t tcp;
} http_server_t;

#endif