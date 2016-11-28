
CC=gcc
CFLAGS=`pkg-config --cflags --libs json-c json` -Isrc/  
SRC = $(wildcard src/*c)
LIB = -lwebsockets -lpthread

all: clean build

build:
	@$(CC) -o bin/StreamerServer.o  $(SRC) $(CFLAGS) $(LIB)
	@echo '*** Build Completed ***'

server:
	@echo '*** Server Running ***'
	@bin/./StreamerServer.o

clean:
	@rm -rf bin/*.o src/*~
	@echo '*** Clean Completed ***'
