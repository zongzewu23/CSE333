CC = g++
CFLAGS = -g -Wall
HEADERS = Vector.h
AR = ar
ARFLAGS = rcs

all: ex9

ex9: ex9.o libvector.a
	$(CC) $(CFLAGS) ex9.o -o ex9 -L. -lvector

libvector.a: Vector.o
	$(AR) $(ARFLAGS) libvector.a Vector.o

%.o: %.cc $(HEADERS)
	$(CC) $(CFLAGS) -c $<

clean: FORCE
	/bin/rm -f *.o *.a *~ ex9

FORCE:
