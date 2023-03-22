#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ftw.h>

long long total_size = 0;

int process_file(const char *path, const struct stat *statbuf, int typeflag) {
    if (!S_ISDIR(statbuf->st_mode)) {
        printf("%lld %s\n", (long long)statbuf->st_size, path);
        total_size += statbuf->st_size;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (ftw(argv[1], process_file, 16) == -1) {
        perror("ftw");
        exit(EXIT_FAILURE);
    }

    printf("Total size: %lld\n", total_size);

    return 0;
}
