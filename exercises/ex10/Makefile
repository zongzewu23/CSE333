CC = g++
CFLAGS = -g -Wall
HEADERS = Vector.h
AR = ar
ARFLAGS = rcs

all: ex10

ex10: ex10.o libvector.a
	$(CC) $(CFLAGS) ex10.o -o ex10 -L. -lvector

libvector.a: Vector.o
	$(AR) $(ARFLAGS) libvector.a Vector.o

%.o: %.cc $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean: FORCE
	/bin/rm -f *.o *.a *~ ex10

FORCE:
