#ifndef GLOBAL_H_
#define GLOBAL_H_

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <uv.h>
#include <http_parser.h>

#include "string.h"
#include "queue.h"
#include "memory.h"
#include "http.h"

// Helpers
#define ERROR(uv_loop) \
    uv_err_t err = uv_last_error(uv_loop); \
    fprintf(stderr, "%s:%d:%s() %s\n", __FILE__, __LINE__, __FUNCTION__, uv_strerror(err));

#define CHECK(action, ret) if (action) { ERROR() return ret; }

#endif
