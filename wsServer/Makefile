CC   = g++
SRC = $(wildcard src/*c src/*cpp)
TARGET = bin/wsServer.o

LIBRARY += pcre
LIBRARY += boost_system
LIBRARY += boost_coroutine
LIBRARY += boost_context
LIBRARY += boost_thread
LIBRARY += boost_chrono
LIBRARY += boost_date_time
LIBRARY += boost_atomic
LIBRARY += crypto
LIBRARY += pthread

LIBRARYDIR += /lib/x86_64-linux-gnu
LIBRARYDIR += /usr/lib/x86_64-linux-gnu
LIBRARYDIR += /lib
LIBRARYDIR += /usr/lib

XLINK_LIBDIR += /lib/x86_64-linux-gnu
XLINK_LIBDIR += /usr/lib/x86_64-linux-gnu

INCLUDEDIR += src/

SYSROOT	= --sysroot=/
DEBUG 	= -g
WARN	= -Wall -Wextra
PTHREAD	= -pthread
STD	= -std=c++11

INCDIR   = $(patsubst %,-I%,$(INCLUDEDIR))
LIBDIR   = $(patsubst %,-L%,$(LIBRARYDIR))
LIB      = $(patsubst %,-l%,$(LIBRARY))
XLINKDIR = $(patsubst %,-Xlinker -rpath-link=%,$(XLINK_LIBDIR))

all: clean
	$(CC) $(STD) $(SRC) -o $(TARGET) $(OPT) $(DEBUG) $(WARN) $(LIBDIR) $(SYSROOT) $(PTHREAD) $(INCDIR) $(XLINKDIR) $(LIB)

clean:
	rm -rf $(TARGET)
