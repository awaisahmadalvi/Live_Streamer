
CC=gcc
CFLAGS=`pkg-config --cflags --libs json-c json` '-lpthread' -Isrc/  

all: clean build

build:
	@$(CC) -o bin/OTAClient.o src/client.c src/clientComm.c src/clientJson.c  $(CFLAGS)
	@echo '*** Build Completed ***'

client:
	@echo '*** Client Running ***'
	@bin/./OTAClient.o

clean:
	@rm -rf bin/*.o src/*~
	@echo '*** Clean Completed ***'

