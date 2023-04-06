#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s liczba_procesów_potomnych\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            printf("PID macierzystego: %d, PID potomka: %d\n", getppid(), getpid());
            exit(EXIT_SUCCESS);
        }
    }

    for (int i = 0; i < n; i++) {
        int status;
        pid_t pid = wait(&status);
        if (pid == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        } else {
            printf("Proces potomny o PID %d zakończył działanie z kodem %d\n", pid, WEXITSTATUS(status));
        }
    }

    printf("%s\n", argv[1]);

    return EXIT_SUCCESS;
}
