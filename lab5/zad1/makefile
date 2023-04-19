CC = gcc -Wall

all: clean
	$(CC) main.c -o main
	./main write "hello"
	./main read

clean:
	rm -f main *.o