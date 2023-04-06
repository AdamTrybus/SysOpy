#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

int requests_num = 0;
int catcher_state = 0;
volatile sig_atomic_t done = 1;

void handler (int signum, siginfo_t* info, void* context) {
    int pid = info->si_pid;
    catcher_state = info->si_value.sival_int;

    if (catcher_state < 1 || catcher_state > 5) {
        fprintf(stderr, "CATCHER | There is no such mode of operation!\n");
    }
    else {
        requests_num++;
        done = 0;
    }

    union sigval value;
    value.sival_int = catcher_state;
    sigqueue(pid, SIGUSR1, value);
}

void print_numbers() {
    for (int i = 0; i <= 100; i++) {
        printf("CATCHER | %d\n", i);
    }
    done = 1;
}

void print_time() {
    time_t raw_time;
    struct tm* time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);
    printf("CATCHER | Current time: %s", asctime(time_info));
}

void print_requests_num() {
    printf("CATCHER | Requests count: %d\n", requests_num);
    done = 1;
}

void end_prog() {
    printf("CATCHER | Exit\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {
    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handler;
    action.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &action, NULL);

    printf("CATCHER | PID: %d\n", getpid());

    while(1) {
        if (done) pause();

        switch(catcher_state) {
            case 1:
                print_numbers();
                break;
            case 2:
                print_time();
                break;
            case 3:
                print_requests_num();
                break;
            case 4:
                while(!done) {
                    print_time();
                    sleep(1);
                }
                done = 1;
                break;
            case 5:
                end_prog();
                break;
            default:
                break;
        }

        fflush(NULL);
    }

    exit(EXIT_SUCCESS);
}
