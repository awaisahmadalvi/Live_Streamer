CC   = gcc
SRC = $(wildcard src/gstreamer/*c src/TCPClient/*c)
TARGET = bin/StreamerClient.o

LIBRARY += gobject-2.0
LIBRARY += gstreamer-1.0
LIBRARY += glib-2.0
LIBRARY += json
LIBRARY += json-c
LIBRARY += pcre

LIBRARYDIR += /lib/x86_64-linux-gnu
LIBRARYDIR += /usr/lib/x86_64-linux-gnu
LIBRARYDIR += /lib
LIBRARYDIR += /usr/lib

XLINK_LIBDIR += /lib/x86_64-linux-gnu
XLINK_LIBDIR += /usr/lib/x86_64-linux-gnu

INCLUDEDIR += src/TCPClient/
INCLUDEDIR += /usr/include/gstreamer-1.0
INCLUDEDIR += /usr/include/json
INCLUDEDIR += /usr/include/glib-2.0
INCLUDEDIR += /usr/lib/x86_64-linux-gnu/glib-2.0/include
INCLUDEDIR += /usr/lib/x86_64-linux-gnu/gstreamer-1.0/include

SYSROOT = --sysroot=/
DEBUG = -g
WARN= -Wall
PTHREAD= -pthread

INCDIR  = $(patsubst %,-I%,$(INCLUDEDIR))
LIBDIR  = $(patsubst %,-L%,$(LIBRARYDIR))
LIB    = $(patsubst %,-l%,$(LIBRARY))
XLINKDIR = $(patsubst %,-Xlinker -rpath-link=%,$(XLINK_LIBDIR))

all: clean
	$(CC) $(SRC) -o $(TARGET) $(OPT) $(DEBUG) $(WARN) $(LIBDIR) $(SYSROOT) $(PTHREAD) $(INCDIR) $(XLINKDIR) $(LIB)

clean:
	rm -rf $(TARGET)