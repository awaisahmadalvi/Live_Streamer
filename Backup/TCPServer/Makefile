
CC=gcc
CFLAGS=`pkg-config --cflags --libs json-c json` '-lpthread' -Isrc/  

all: clean build

build:
	@$(CC) -o bin/server.o src/server.c src/srvrComm.c src/srvrJson.c $(CFLAGS)
	@echo '*** Build Completed ***'

server:
	@echo '*** Server Running ***'
	@bin/./server.o

clean:
	@rm -rf bin/*.o src/*~
	@echo '*** Clean Completed ***'
