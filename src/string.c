#include "global.h"

#define START_SIZE 64

void str_free(str_t* str) {
	if (str->buf.base != NULL) {
		free(str->buf.base);
		memset(str, 0, sizeof(str_t));
	}

}

void str_reserve(str_t* str, size_t size) {
	if (str->allocated >= size)
		return;

	if (str->buf.base == NULL) {
		str->buf.base = malloc(size);
	} else {
		str->buf.base = realloc(str->buf.base, size);
	}

	str->allocated = size;
}

void _reserve(str_t* str, size_t new_len) {
	size_t allocated = str->allocated;

	if (allocated == 0)
		allocated = START_SIZE;

	while (new_len >= allocated)
		allocated *= 2;

	str_reserve(str, allocated);
}

void str_set(str_t* str, const char* src) {
	size_t len = strlen(src);

	_reserve(str, len + 1);
	strncpy(str->buf.base, src, len + 1);
	str->buf.len = len;
}

void str_append(str_t* str, const char* data) {
	str_append_len(str, data, strlen(data));
}

void str_append_len(str_t* str, const char* data, size_t len) {
	// TODO: assert?
	if (len == 0)
		return;

	// Extend buffer if necessary
	size_t new_len = str->buf.len + len;
	_reserve(str, new_len + 1);

	// Copy string
	char* buf = (char*)str->buf.base;
	memcpy(buf + str->buf.len, data, len);
	buf[new_len] = '\0';
	str->buf.len = new_len;
}

void str_append_buf(str_t* str, uv_buf_t src) {
	if (src.len == 0)
		return;

	str_append_len(str, src.base, src.len);
}

void str_format(str_t* str, const char* fmt, ...) {
	va_list argp;
	va_start(argp, fmt);

	size_t len = vsnprintf(NULL, 0, fmt, argp) + 1;
	_reserve(str, len);

	va_start(argp, fmt);
	vsnprintf(str->buf.base, len, fmt, argp);
	str->buf.len = len - 1;
}

void str_cat_format(str_t* str, const char* fmt, ...) {
	va_list argp;
	va_start(argp, fmt);

	size_t len = vsnprintf(NULL, 0, fmt, argp) + 1;
	size_t new_len = str->buf.len + len;
	_reserve(str, new_len);

	// TODO: Fix me
	va_start(argp, fmt);
	char* buf = (char*)str->buf.base;
	vsnprintf(buf + str->buf.len, len + 1, fmt, argp);
	str->buf.len = new_len - 1;
}
