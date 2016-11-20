
CC=gcc
CFLAGS=`pkg-config --cflags --libs json-c json gstreamer-1.0 glib-2.0 gobject-2.0 x11`

all: clean build

build: src/flv.c
	@$(CC) -o bin/streamer.o src/flv.c $(CFLAGS)
	@echo '*** Build Completed ***'

run:
	@bin/./streamer.o
	@echo '*** Run Completed ***'

clean:
	@rm -rf bin/*.o src/*~ /root/videos/*.flv
	@echo '*** Clean Completed ***'
