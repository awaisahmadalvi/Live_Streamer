
CC=gcc
CFLAGS=`pkg-config --cflags --libs gstreamer-1.0 glib-2.0 glib-2.0 gobject-2.0 x11`

all: build run

build: flv.c
	@$(CC) -o streamer.o flv.c $(CFLAGS)
	@echo '*** Build Completed ***'

run:
	@./streamer.o
	@echo '*** Run Completed ***'

clean:
	@rm *.o *~ *.flv streamer
	@echo '*** Clean Completed ***'
