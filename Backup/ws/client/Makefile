
all: example-client

example-client: src/client.c
	@gcc src/client.c -o bin/client -lpthread -lwebsockets
clean:
	@rm -rf bin/client
