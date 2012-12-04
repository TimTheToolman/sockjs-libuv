#ifndef MEMORY_H_
#define MEMORY_H_

uv_buf_t memory_alloc(uv_handle_t* handle, size_t size);
uv_buf_t memory_realloc(uv_buf_t buf, size_t new_size);
uv_buf_t memory_free(uv_buf_t buf);

#endif
