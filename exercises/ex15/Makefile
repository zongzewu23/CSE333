CC = g++
CFLAGS = -g -Wall -std=c++17

HEADERS = file.h network.h
SOURCES = ex15.cc file.cc network.cc
OBJECTS = $(SOURCES:.cc=.o)

all: ex15

ex15: $(OBJECTS)
	$(CC) $(CFLAGS) -o ex15 $(OBJECTS)

ex15.o: ex15.cc file.h network.h
	$(CC) $(CFLAGS) -c ex15.cc -o ex15.o

file.o: file.cc file.h
	$(CC) $(CFLAGS) -c file.cc -o file.o

network.o: network.cc network.h
	$(CC) $(CFLAGS) -c network.cc -o network.o

clean:
	rm -f *.o *~ ex15