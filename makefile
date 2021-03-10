BINDIR=.
CC=gcc
CFLAGS=-c -g -D_DEBUG_LEVEL_=4 -m64 -fno-builtin -Werror -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D__LINUX__
includedir=-I./ -I/usr/local/include/hiredis/
LDFLAGS=-m64
libdir=-L/usr/local/lib/
LDSHARED=-lhiredis -lstdc++

LS_SRC= main.cpp redislist.cpp wredis.cpp redishash.cpp

.cpp.o: 
	$(CC) $(CFLAGS) $(includedir) $< -o $@

all: $(BINDIR)/a.out clean

$(BINDIR)/a.out: $(LS_SRC:.cpp=.o)
	$(CC) $(LDFLAGS) $(LS_SRC:.cpp=.o) -o $@ $(libdir) $(LDSHARED)

.PHONY: clean
clean:
	@rm -f *.o
