#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s ścieżka_katalogu\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("%s", argv[0]);  // wypisanie nazwy programu bez znaku nowego wiersza
    fflush(stdout);  // wymuszenie wypisania zawartości bufora wyjścia

    setbuf(stdout, NULL);  // wyłączenie buforowania wyjścia

    execl("/bin/ls", "ls", argv[1], NULL);  // wywołanie programu /bin/ls z argumentem argv[1]

    perror("execl");  // w przypadku niepowodzenia wywołania funkcji execl, wypisanie błędu

    return EXIT_FAILURE;  // zwrócenie kodu błędu
}
