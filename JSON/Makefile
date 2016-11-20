
CC=gcc
CFLAGS=`pkg-config --cflags --libs json-c json glib-2.0 gobject-2.0 x11`

all: clean build

build: json.c
	@$(CC) -o a.out json.c $(CFLAGS)
	@echo '*** Build Completed ***'

run:
	@./a.out
	@echo '*** Run Completed ***'

clean:
	@rm -rf a.out *~ 
	@echo '*** Clean Completed ***'
