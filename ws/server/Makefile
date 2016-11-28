
all: example-PlayerServer

example-PlayerServer: src/PlayerServer.c
	@gcc src/PlayerServer.c -o bin/PlayerServer -lwebsockets

clean:
	@rm -rf bin/PlayerServer
