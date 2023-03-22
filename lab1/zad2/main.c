#include <stdio.h>
#include <time.h>
#include <sys/times.h>
#include <string.h>
#include <stdlib.h>
#include "mlib.h"
#define BUFFER_SIZE 1024


MemoryBlocks* memory;
int exists = 0;

int main() {

    char command[BUFFER_SIZE];
    char input[BUFFER_SIZE];
    struct timespec timespec_start, timespec_end;
    struct tms tms_start, tms_end;

    while (1) {
        printf("Enter command: ");
        fgets(input, BUFFER_SIZE, stdin);

        int arg = -1;

        clock_gettime(CLOCK_REALTIME, &timespec_start);
        times(&tms_start);
        sscanf(input,"%s %d", command, &arg);
        if (strcmp(command, "init") == 0 ) {
            if(exists){
                destroy(memory);
            }
            memory = init(arg);
            exists = 1;
            printf("init %d \n", arg);
        } else if (strcmp(command, "show") == 0) {
            char *block = show(memory,arg);
            printf("Block %d: %s", arg, block);
            printf("show %d \n", arg);
        } else if (strcmp(command, "delete") == 0) {
            delete_block(memory,arg);
            printf("delete %d \n", arg);
        } else if (strcmp(command, "count") == 0) {
            char filename[100];
            sscanf(input,"%s %s", command, filename);
            filename[strcspn(filename, "\n")] = '\0';
            count(memory, filename);
            printf("count %s \n", filename);
            
        } else if (strcmp(command, "destroy") == 0) {
            destroy(memory);
            exists = 0;
            printf("destroy \n");
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("błędna komenda \n");
        }

        clock_gettime(CLOCK_REALTIME, &timespec_end);
        times(&tms_end);
        printf("Czasy wykonania:\n");
        printf("Użytkownik: %ld\n", tms_end.tms_utime - tms_start.tms_utime);
        printf("Systemowy: %ld\n", tms_end.tms_stime - tms_start.tms_stime);
        printf("Rzeczywisty: %ld nsec\n\n", timespec_end.tv_nsec - timespec_start.tv_nsec);
        
    }

    return 0;
}
