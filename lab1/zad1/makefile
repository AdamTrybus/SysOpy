CC = gcc -Wall

all:
	make test_static
	make test_shared

#zad1
mlib.o_sh:
	$(CC) -fPIC -c mlib.c -g -o mlib.o
mlib.o_st:
	$(CC) -c mlib.c -g -o mlib.o
mlib.a: mlib.o_st
	ar cr libmlib.a mlib.o
mlib.so: mlib.o_sh
	$(CC) -fPIC -shared -o libmlib.so -g mlib.o

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so*
