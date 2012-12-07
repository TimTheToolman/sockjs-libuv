#ifndef STRING_H_
#define STRING_H_

typedef struct {
	uv_buf_t buf;
	size_t allocated;
} str_t;

void str_reserve(str_t* str, size_t size);
void str_free(str_t* str);

void str_set(str_t* str, const char* src);

void str_append(str_t* str, const char* data);
void str_append_len(str_t* str, const char* data, size_t len);
void str_append_buf(str_t* str, uv_buf_t src);

void str_format(str_t* str, const char* fmt, ...);
void str_cat_format(str_t* str, const char* fmt, ...);

#define str_len(str) \
	((str)->buf.len)

#define str_empty(str)	\
	((str)->buf.len == 0)

#endif
