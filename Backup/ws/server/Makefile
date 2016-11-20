
all: example-server

example-server: src/server.c
	@gcc src/server.c -o bin/server -lwebsockets

clean:
	@rm -rf bin/server
