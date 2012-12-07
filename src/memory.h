#ifndef MEMORY_H_
#define MEMORY_H_

uv_buf_t memory_uv_alloc(uv_handle_t* handle, size_t size);
void memory_uv_free(uv_buf_t buf);

#endif
