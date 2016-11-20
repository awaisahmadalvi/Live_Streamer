
CC=gcc
CFLAGS=`pkg-config --cflags --libs json-c json` '-lpthread' -Isrc/  

SRC_FILES=src/Json.c src/main.c src/OTAServer.c src/PlayerServer.c src/StreamerServer.c src/OTASrvrComm.c src/PlayerSrvrComm.c src/StreamerSrvrComm.c

all: clean build

build:
	@$(CC) -o bin/server.o  $(SRC_FILES) $(CFLAGS)
	@echo '*** Build Completed ***'

server:
	@echo '*** Server Running ***'
	@bin/./server.o

clean:
	@rm -rf bin/*.o src/*~
	@echo '*** Clean Completed ***'
