#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

int is_parent = 1;

void signal_handler(int sig_num) {
    printf("(%d) Received signal: %d\n", getpid(), sig_num);
}

void mask_signal() {
    sigset_t mask_set;
    if (sigemptyset(&mask_set) == -1) {
        perror("sigemptyset");
        exit(EXIT_FAILURE);
    }
    if (sigaddset(&mask_set, SIGUSR1) == -1) {
        perror("sigaddset");
        exit(EXIT_FAILURE);
    }
    if (sigprocmask(SIG_BLOCK, &mask_set, NULL) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }
}

void check_pending() {
    sigset_t pending_set;
    if (sigpending(&pending_set) == -1) {
        perror("sigpending");
        exit(EXIT_FAILURE);
    }
    if (sigismember(&pending_set, SIGUSR1)) {
        printf("(%d) Signal is pending\n", getpid());
    }
    else {
        printf("(%d) Signal is not pending\n", getpid());
    }
}

void raise_signal() {
    printf("(%d) Raising signal\n", getpid());
    if (raise(SIGUSR1) == -1) {
        perror("raise");
        exit(EXIT_FAILURE);
    }
}

void process_signal(char* parse_option) {
    if (strcmp(parse_option, "ignore") == 0) {
        if (signal(SIGUSR1, SIG_IGN) == SIG_ERR) {
            perror("signal");
            exit(EXIT_FAILURE);
        }
        raise_signal();
    }
    else if (strcmp(parse_option, "handler") == 0) {
        if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
            perror("signal");
            exit(EXIT_FAILURE);
        }
        raise_signal();
    }
    else if (strcmp(parse_option, "mask") == 0) {
        if (is_parent) {
            mask_signal();
            raise_signal();
        }
    }
    else if (strcmp(parse_option, "pending") == 0) {
        if (is_parent) {
            mask_signal();
            raise_signal();
        }
        check_pending();
    }
    else {
        fprintf(stderr, "Unknown argument: %s\n", parse_option);
        exit(EXIT_FAILURE);
    }
    fflush(NULL);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Wrong arguments number!\n");
        exit(EXIT_FAILURE);
    }

    process_signal(argv[1]);

    is_parent = fork();
    if (!is_parent) {
        process_signal(argv[1]);
    }
    else {
        wait(NULL);
    }

    exit(EXIT_SUCCESS);
}
