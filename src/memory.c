#include "global.h"

uv_buf_t memory_uv_alloc(uv_handle_t* handle, size_t size) {
	return uv_buf_init(malloc(size), (unsigned int)size);
}

void memory_uv_free(uv_buf_t buf) {
	free(buf.base);
}
