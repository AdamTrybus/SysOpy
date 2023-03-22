#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 4096

void replace_char(FILE *input_file, FILE *output_file, char a, char b) {
    char buffer[BUFFER_SIZE];
    size_t read_count;

    while ((read_count = fread(buffer, sizeof(char), BUFFER_SIZE, input_file)) > 0) {
        for (size_t i = 0; i < read_count; i++) {
            if (buffer[i] == a) {
                buffer[i] = b;
            }
        }
        fwrite(buffer, sizeof(char), read_count, output_file);
    }
}

void replace_char_sys(int input_fd, int output_fd, char a, char b) {
    char buffer[BUFFER_SIZE];
    ssize_t read_count;

    while ((read_count = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < read_count; i++) {
            if (buffer[i] == a) {
                buffer[i] = b;
            }
        }
        write(output_fd, buffer, read_count);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s search_char replace_char input_file output_file\n", argv[0]);
        return EXIT_FAILURE;
    }

    char a = argv[1][0];
    char b = argv[2][0];
    char *input_file_path = argv[3];
    char *output_file_path = argv[4];
    char output_file_path1[100], output_file_path2[100];
    sprintf(output_file_path1, "%s%s", output_file_path, "1");
    sprintf(output_file_path2, "%s%s", output_file_path, "2");

    FILE *input_file = fopen(input_file_path, "r");
    if (input_file == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    FILE *output_file = fopen(output_file_path1, "w");
    if (output_file == NULL) {
        perror("Error opening output file");
        fclose(input_file);
        return EXIT_FAILURE;
    }

    replace_char(input_file, output_file, a, b);
    fclose(input_file);
    fclose(output_file);

    int input_fd = open(input_file_path, O_RDONLY);
    if (input_fd < 0) {
        perror("Error opening input file (sys)");
        return EXIT_FAILURE;
    }

    int output_fd = open(output_file_path2, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    if (output_fd < 0) {
        perror("Error opening output file (sys)");
        close(input_fd);
        return EXIT_FAILURE;
    }

    replace_char_sys(input_fd, output_fd, a, b);
    close(input_fd);
    close(output_fd);
}

