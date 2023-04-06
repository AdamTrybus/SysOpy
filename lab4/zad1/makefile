CC = gcc -Wall

main: clean zad1 test

zad1:
	$(CC) main.c -o main

test:
	echo ">>> FORK <<<" > "report2.txt"
	echo "HANDLER:" >> "report2.txt"
	./main handler >> "report2.txt"
	echo "IGNORE:" >> "report2.txt"
	./main ignore >> "report2.txt"
	echo "MASK:" >> "report2.txt"
	./main mask >> "report2.txt"
	echo "PENDING:" >> "report2.txt"
	./main pending >> "report2.txt"

clean:
	rm -f main *.o