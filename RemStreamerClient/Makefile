ARM_PREFIX= aarch64-linux-gnu-
CC   = $(ARM_PREFIX)gcc
SRC = $(wildcard src/gstreamer/*c src/TCPClient/*c)
TARGET = bin/StreamerClient.o

LIBRARY += gobject-2.0
LIBRARY += gstreamer-1.0
LIBRARY += glib-2.0
LIBRARY += json
LIBRARY += json-c
LIBRARY += pcre

LIBRARYDIR += $(HOME)/OdroidFS/lib/aarch64-linux-gnu
LIBRARYDIR += $(HOME)/OdroidFS/usr/lib/aarch64-linux-gnu
LIBRARYDIR += $(HOME)/OdroidFS/lib
LIBRARYDIR += $(HOME)/OdroidFS/usr/lib

XLINK_LIBDIR += $(HOME)/OdroidFS/lib/aarch64-linux-gnu
XLINK_LIBDIR += $(HOME)/OdroidFS/usr/lib/aarch64-linux-gnu

INCLUDEDIR += src/TCPClient/
INCLUDEDIR += $(HOME)/OdroidFS/usr/include/gstreamer-1.0
INCLUDEDIR += $(HOME)/OdroidFS/usr/include/json
INCLUDEDIR += $(HOME)/OdroidFS/usr/include/glib-2.0
INCLUDEDIR += $(HOME)/OdroidFS/usr/lib/aarch64-linux-gnu/glib-2.0/include
INCLUDEDIR += $(HOME)/OdroidFS/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include

SYSROOT = --sysroot=$(HOME)/OdroidFS/
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