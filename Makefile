all:	vendor/libuv/uv.a vendor/http-parser/http-parser.a

clean:
		rm vendor/libuv/uv.a
		rm vendor/http-parser/http-parser.a

# Dependencies
vendor/libuv/uv.a:
		$(MAKE) -C vendor/libuv

vendor/http-parser/http-parser.a:
		$(MAKE) -C vendor/http-parser package
