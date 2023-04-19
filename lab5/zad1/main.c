#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

void print_error(const char* message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void write_cow_message(const char* message) {
    FILE* cow_proc = popen("cowsay", "w");
    if (cow_proc == NULL) {
        print_error("Error: failed to execute cowsay command");
    }
    fwrite(message, sizeof(char), strlen(message), cow_proc);
    pclose(cow_proc);
}

void read_fortune_cookie() {
    char buffer[BUFFER_SIZE];
    FILE* fortune_proc = popen("fortune", "r");
    if (fortune_proc == NULL) {
        print_error("Error: failed to execute fortune command");
    }
    size_t size = fread(buffer, sizeof(char), BUFFER_SIZE - 1, fortune_proc);
    buffer[size] = '\0';
    printf("%s\n", buffer);
    pclose(fortune_proc);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_error("Error: wrong number of arguments!\n");
    }
    if (strcmp(argv[1], "write") == 0) {
        if (argc != 3) {
            print_error("Error: wrong number of arguments for 'write' mode\n");
        }
        write_cow_message(argv[2]);
    } else if (strcmp(argv[1], "read") == 0) {
        read_fortune_cookie();
    } else {
        print_error("Error: unknown argument\nUsage: cowsay_cli [w/write/r/read]\n");
    }
    return 0;
}
