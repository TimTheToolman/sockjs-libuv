#ifndef MEMORY_H_
#define MEMORY_H_

uv_buf_t memory_uv_alloc(uv_handle_t* handle, size_t size);
uv_buf_t memory_alloc(size_t size);
uv_buf_t memory_realloc(uv_buf_t buf, size_t new_size);
void memory_free(uv_buf_t buf);

#define memory_release_buf(buf)	if (buf.base != NULL) { memory_free(buf); buf.base = NULL; buf.len = 0; }

#endif
