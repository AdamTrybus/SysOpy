#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

void copy_reverse_bytes(char* input_file, char* output_file, int block_size) {
    int fd_in = open(input_file, O_RDONLY);
    if (fd_in == -1) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }
    int fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd_out == -1) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    // Ustawiamy wskaźnik pliku na koniec
    off_t input_file_size = lseek(fd_in, 0, SEEK_END);

    // Iterujemy po blokach danych od końca do początku
    for (off_t i = input_file_size - block_size; i >= 0; i -= block_size) {
        // Ustawiamy wskaźnik pliku wejściowego na pozycji i
        lseek(fd_in, i, SEEK_SET);
        // Odczytujemy blok danych o wielkości block_size
        char block[block_size];
        ssize_t bytes_read = read(fd_in, block, block_size);
        if (bytes_read == -1) {
            perror("Error reading from input file");
            exit(EXIT_FAILURE);
        }
        // Odwracamy kolejność bajtów w bloku
        for (int j = 0; j < block_size / 2; j++) {
            char temp = block[j];
            block[j] = block[block_size - j - 1];
            block[block_size - j - 1] = temp;
        }
        // Zapisujemy blok danych do pliku wyjściowego
        ssize_t bytes_written = write(fd_out, block, block_size);
        if (bytes_written == -1) {
            perror("Error writing to output file");
            exit(EXIT_FAILURE);
        }
    }
    // Odczytujemy pozostałe bajty, które nie zostały odczytane w blokach
    off_t remaining_bytes = input_file_size % block_size;
    if (remaining_bytes > 0) {
        lseek(fd_in, 0, SEEK_SET);
        char last_block[remaining_bytes];
        ssize_t bytes_read = read(fd_in, last_block, remaining_bytes);
        if (bytes_read == -1) {
            perror("Error reading from input file");
            exit(EXIT_FAILURE);
        }
        // Odwracamy kolejność bajtów w bloku
        for (int j = 0; j < remaining_bytes / 2; j++) {
            char temp = last_block[j];
            last_block[j] = last_block[remaining_bytes - j - 1];
            last_block[remaining_bytes - j - 1] = temp;
        }
        // Zapisujemy blok danych do pliku wyjściowego
        ssize_t bytes_written = write(fd_out, last_block, remaining_bytes);
        if (bytes_written == -1) {
            perror("Error writing to output file");
            exit(EXIT_FAILURE);
        }
    }

    if (close(fd_in) == -1) {
        perror("Error closing input file");
        exit(EXIT_FAILURE);
    }
   
}

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s input_file output_file block_size\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    char* input_file = argv[1];
    char* output_file = argv[2];
    int block_size = atoi(argv[3]);

    clock_t start_time = clock();
    copy_reverse_bytes(input_file, output_file, block_size);
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Time elapsed: %f seconds\n", elapsed_time);

    return 0;
}