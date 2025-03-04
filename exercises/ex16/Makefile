CC = g++
CFLAGS = -g -Wall -std=c++17

HEADERS = SocketUtil.h
SOURCES = ex16.cc SocketUtil.cc
OBJECTS = $(SOURCES:.cc=.o)

all: ex16

ex16: $(OBJECTS)
	$(CC) $(CFLAGS) -o ex16 $(OBJECTS)

ex16.o: ex16.cc SocketUtil.h
	$(CC) $(CFLAGS) -c ex16.cc -o ex16.o

SocketUtil.o: SocketUtil.cc SocketUtil.h
	$(CC) $(CFLAGS) -c SocketUtil.cc -o SocketUtil.o

clean:
	rm -f *.o *~ ex16