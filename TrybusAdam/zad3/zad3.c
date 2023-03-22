#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

int main() {
    DIR *dir;
    struct dirent *entry;
    struct stat buf;
    long long total_size = 0;

    dir = opendir(".");
    if (!dir) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (stat(entry->d_name, &buf) == -1) {
            perror("stat");
            continue;
        }

        if (!S_ISDIR(buf.st_mode)) {
            printf("%lld %s\n", buf.st_size, entry->d_name);
            total_size += buf.st_size;
        }
    }

    closedir(dir);

    printf("Total size: %lld\n", total_size);

    return 0;
}
