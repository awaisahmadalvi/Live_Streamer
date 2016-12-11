ARM_PREFIX= aarch64-linux-gnu-
CROSSCC   = $(ARM_PREFIX)gcc
SRC = $(wildcard src/*c)
CROSSTARGET = bin/cross/OTAClient.o
TARGET = bin/OTAClient.o

LIBRARY += json
LIBRARY += json-c

LIBRARYDIR += $(HOME)/OdroidFS/lib/aarch64-linux-gnu
LIBRARYDIR += $(HOME)/OdroidFS/usr/lib/aarch64-linux-gnu
LIBRARYDIR += $(HOME)/OdroidFS/lib
LIBRARYDIR += $(HOME)/OdroidFS/usr/lib

XLINK_LIBDIR += $(HOME)/OdroidFS/lib/aarch64-linux-gnu
XLINK_LIBDIR += $(HOME)/OdroidFS/usr/lib/aarch64-linux-gnu

INCLUDEDIR 	 += src/
INCLUDEDIR 	 += $(HOME)/OdroidFS/usr/include/json

SYSROOT = --sysroot=$(HOME)/OdroidFS/
DEBUG 	= -g
WARN 	= -Wall
PTHREAD = -pthread

INCDIR  = $(patsubst %,-I%,$(INCLUDEDIR))
LIBDIR  = $(patsubst %,-L%,$(LIBRARYDIR))
LIB	    = $(patsubst %,-l%,$(LIBRARY))
XLINKDIR = $(patsubst %,-Xlinker -rpath-link=%,$(XLINK_LIBDIR))

CC=gcc
CFLAGS=`pkg-config --cflags --libs json-c json` '-lpthread' -Isrc/

all: clean cross local

cross:
	@$(CROSSCC) $(SRC) -o $(CROSSTARGET) $(OPT) $(DEBUG) $(WARN) $(LIBDIR) $(SYSROOT) $(PTHREAD) $(INCDIR) $(XLINKDIR) $(LIB)
	@echo '*** CROSS-Build Completed ***'
	
local:
	@$(CC) -o $(TARGET) $(SRC) $(CFLAGS)
	@echo '*** Build Completed ***'

clean:
	@rm -rf $(CROSSTARGET) $(TARGET) src/*~
	@echo '*** Clean Completed ***'
