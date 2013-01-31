CSTDFLAG= 
CFLAGS += -g -Ivendor/libuv/include -Ivendor/http-parser
LINKFLAGS=-lm -lpthread -lrt

OBJS=src/server.o src/client.o src/memory.o src/http.o src/queue.o src/router.o 
INCLUDES=$(wildcard src/*.h)

# Platform customizations
uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

ifeq (Darwin,$(uname_S))
LINKFLAGS+=-framework CoreServices
endif

# Rules
all:	vendor/libuv/libuv.a vendor/http-parser/libhttp_parser.a server

server: $(OBJS)
		$(CC) -o src/server $(OBJS) vendor/libuv/libuv.a vendor/http-parser/libhttp_parser.a $(LINKFLAGS)

clean:
		rm -f src/server

src/%.o: src/%.c ${INCLUDES}
		$(CC) $(CSTDFLAG) $(CFLAGS) -c $< -o $@

# Dependencies
vendor/libuv/libuv.a:
		$(MAKE) -C vendor/libuv

vendor/http-parser/libhttp_parser.a:
		$(MAKE) -C vendor/http-parser package
