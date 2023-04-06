#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

typedef void (*handler_t)(int, siginfo_t*, void*);

int call_id;
int call_depth;

void set_action(int flag, handler_t handler) {
    struct sigaction action = {0};
    action.sa_sigaction = handler;
    action.sa_flags = flag;
    sigemptyset(&action.sa_mask);
    sigaction(SIGUSR1, &action, NULL);
    call_id = 0;
    call_depth = 0;
}

void siginfo_handler(int signum, siginfo_t* info, void* context) {
    printf("Signal number: %d\n", info->si_signo);
    printf("PID: %d\n", info->si_pid);
    printf("UID: %d\n", info->si_uid);
    printf("Addr: %p\n", info->si_addr);
    printf("Code: %d\n", info->si_code);
}   

void depth_handler(int signum, siginfo_t* info, void* context) {
    printf("Start handling - call_id: %d, call_depth: %d\n", call_id, call_depth);
    call_id++;
    call_depth++;
    if (call_id < 4) {
        kill(getpid(), SIGUSR1);
    }
    call_depth--;
    printf("End handling - call_id: %d, call_depth: %d\n", call_id-1, call_depth);
}

int main(int argc, char** argv) {
    if (argc != 1) {
        fprintf(stderr, "Wrong number of arguments!\n");
        exit(EXIT_FAILURE);
    }

    // siginfo
    printf(">> SIGINFO <<\n");
    printf("PARENT:\n");
    set_action(SA_SIGINFO, siginfo_handler);
    kill(getpid(), SIGUSR1);
    printf("CHILD:\n");
    if (fork() == 0) {
        kill(getpid(), SIGUSR1);
        exit(EXIT_SUCCESS);
    }
    else {
        wait(NULL);
    }
    printf("\n");

    // restart
    printf(">> RESTART <<\n");
    set_action(SA_RESTART, depth_handler);
    kill(getpid(), SIGUSR1);
    printf("\n");

    // nodefer
    printf(">> NODEFER <<\n");
    set_action(SA_NODEFER, depth_handler);
    kill(getpid(), SIGUSR1);

    return 0;
}
