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

#zad2
main_st:
	make mlib.a
	$(CC) -o main -g main.c ../zad3/libmlib.a
	

main_sh:
	make mlib.so
	$(CC) -o main -g main.c ../zad3/libmlib.so

#zad3a
test_static:
	make main_st
	./main < test.txt > results_static.txt

test_shared:
	make main_sh
	./main < test.txt > results_shared.txt

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so*
