#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define NUM_ARGS_REQUIRED 3

volatile sig_atomic_t sig_received;

void handler(int signum) {
    sig_received = 1;
}

int main(int argc, char** argv) {
    if (argc < NUM_ARGS_REQUIRED) {
        fprintf(stderr, "Error: Wrong number of arguments\nUsage: %s <pid> <state_1> <state_2> ... <state_n>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int catcher_pid = atoi(argv[1]);

    struct sigaction action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGUSR1, &action, NULL) == -1) {
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }

    for (int i = 2; i < argc; i++) {
        sig_received = 0;
        int state = atoi(argv[i]);

        sigval_t sig_val = { .sival_int = state };
        if (sigqueue(catcher_pid, SIGUSR1, sig_val) == -1) {
            perror("sigqueue error");
            exit(EXIT_FAILURE);
        }

        printf("Signal sent.. | State: %d\n", state);

        while (!sig_received) {
            pause();
        }
        printf("Signal received..\n");
    }

    return 0;
}
