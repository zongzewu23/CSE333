CC = g++
CFLAGS = -g -Wall -std=c++17

HEADERS = Coordinate.h Cartesian.h Polar.h
SOURCES = ex13.cc Cartesian.cc Polar.cc
OBJECTS = $(SOURCES:.cc=.o)

all: ex13

ex13: $(OBJECTS)
	$(CC) $(CFLAGS) -o ex13 $(OBJECTS)

ex13.o: ex13.cc Coordinate.h Cartesian.h Polar.h
	$(CC) $(CFLAGS) -c ex13.cc -o ex13.o

Cartesian.o: Cartesian.cc Cartesian.h Coordinate.h
	$(CC) $(CFLAGS) -c Cartesian.cc -o Cartesian.o

Polar.o: Polar.cc Polar.h Coordinate.h
	$(CC) $(CFLAGS) -c Polar.cc -o Polar.o

clean:
	rm -f *.o *~ ex13
