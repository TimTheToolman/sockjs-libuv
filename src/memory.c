#include <stdlib.h>
#include <uv.h>

uv_buf_t memory_alloc(uv_handle_t* handle, size_t size) {
	return uv_buf_init(malloc(size), size);
}

uv_buf_t memory_realloc(uv_buf_t buf, size_t new_size) {
	if (buf.base != NULL) {
		buf.base = realloc(buf.base, new_size);
	} else {
		buf.base = malloc(new_size);
	}
	buf.len = new_size;
	return buf;
}

uv_buf_t memory_free(uv_buf_t buf) {
	if (buf.base != NULL) {
		free(buf.base);
		buf.base = NULL;
		buf.len = 0;
	}
	return buf;
}
