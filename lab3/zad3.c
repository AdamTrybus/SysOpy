#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>

void traverse_directory(const char *path, const char *search_string);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <directory_path> <search_string>\n", argv[0]);
        return 1;
    }
    traverse_directory(argv[1], argv[2]);
    return 0;
}

void traverse_directory(const char *path, const char *search_string) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_info;

    dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char file_path[255];
        strcpy(file_path, path);
        strcat(file_path, "/");
        strcat(file_path, entry->d_name);

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        if (lstat(file_path, &file_info) == -1) {
            perror("lstat");
            continue;
        }

        if (S_ISDIR(file_info.st_mode)) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                continue;
            } else if (pid == 0) {
                traverse_directory(file_path, search_string);
                exit(0);
            }
        } else if (S_ISREG(file_info.st_mode)) {
            if (access(file_path, R_OK) == -1) {
                perror("access");
                continue;
            }

            FILE *fp;
            char *line = NULL;
            size_t len = 0;
            ssize_t read;
            fp = fopen(file_path, "r");
            if (fp == NULL) {
                perror("fopen");
                continue;
            }
            while ((read = getline(&line, &len, fp)) != -1) {
                if (strncmp(line, search_string, strlen(search_string)) == 0) {
                    printf("%s (%d)\n", file_path, getpid());
                    break;
                }
            }
            free(line);
            fclose(fp);
        }
    }

    if (closedir(dir) == -1) {
        perror("closedir");
        return;
    }
}
