#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>

#define CHAIR_SEM_ID 0
#define WAITING_SEM_ID 1
#define TIMES_SEM_ID 2

int M, N, P, F;
int *service_times; // ns
int semDesc;
int shmDesc;
int run = 1;
key_t key;
struct sembuf sem_buf;
char time_msg[16];

void set_time() {
    time_t current_time = time(NULL);
    struct tm *local_time = localtime(&current_time);
    strftime(time_msg, sizeof(time_msg), "[%T]", local_time);
}


void signal_handler(int signum) {
	run = 0;
}

void hairdresser_sig_handler(int signum) {
	run = 0;
	exit(EXIT_SUCCESS);
}

void client_loop() {
    int last_client = 0;
    int *times;
    int service_number, time;
    while (run) {
        // Random delay before a new client arrives
        sleep(rand() % 4);
        // Randomly select a service number and get the service time
        service_number = rand() % F;
        time = service_times[service_number];

        // Print message with time stamp and service details
        set_time();
        printf("%s New client has arrived. Service number: %d, time: %fs\n", time_msg, service_number + 1, time / 1000.0);

        // Wait for access to shared memory for times array
        sem_buf.sem_num = TIMES_SEM_ID;
        sem_buf.sem_op = -1;
        semop(semDesc, &sem_buf, 1);

        // Attach to shared memory segment for times array
        times = shmat(shmDesc, NULL, 0);

        // Check if there is an empty place in the queue
        if (times[last_client + 1] == -1) {
            // Add the new client to the queue
            times[last_client + 1] = time;
            // Move the last_client pointer to the next available place in the queue
            last_client = (last_client + 1) % P;

            // Release access to shared memory for waiting semaphore
            sem_buf.sem_num = WAITING_SEM_ID;
            sem_buf.sem_op = 1;
            semop(semDesc, &sem_buf, 1);

            // Print message with time stamp about taking a place in the queue
            set_time();
            printf("%s Client takes a place in the queue\n", time_msg);
        } else {         
            // Print message with time stamp about no empty place in queue
            set_time();
            printf("%s No empty place in the queue, leaving..\n", time_msg);
        }
        shmdt(times);
        sem_buf.sem_num = TIMES_SEM_ID;
        sem_buf.sem_op = 1;
        semop(semDesc, &sem_buf, 1);
    }
}


void hairdresser_loop(int id) {
	int *times;
	set_time();
	printf("%s Hairdresser [%d] started working...\n", time_msg, id);
	while(run) {
		sem_buf.sem_num = CHAIR_SEM_ID;
		sem_buf.sem_op = -1;
		semop(semDesc, &sem_buf, 1);

		sem_buf.sem_num = WAITING_SEM_ID;
		sem_buf.sem_op = -1;
		semop(semDesc, &sem_buf, 1);
		set_time();
		printf("%s Hairdresser [%d] has reserved chair, taking client..\n", time_msg, id);

		times = shmat(shmDesc, NULL, 0);
		sem_buf.sem_num = TIMES_SEM_ID;
		sem_buf.sem_op = -1;
		semop(semDesc, &sem_buf, 1);

		int time = times[times[0]+1];
		times[times[0]+1] = -1;
		times[0] = (times[0] + 1) % P;
		shmdt(times);
		sem_buf.sem_op = 1;
		semop(semDesc, &sem_buf, 1);

		usleep(time);
		set_time();
		printf("%s Hairdresser [%d] finished\n", time_msg, id);
		sem_buf.sem_num = CHAIR_SEM_ID;
		sem_buf.sem_op = 1;
		semop(semDesc, &sem_buf, 1);
	}
}

int main(int argc, char** argv) {
    // Validate command-line arguments
    if (argc < 5) {
        fprintf(stderr, "Error | correct input pattern [M N P F]\n");
        exit(EXIT_FAILURE);
    }

    // Parse command-line arguments
    M = atoi(argv[1]);
    N = atoi(argv[2]);
    P = atoi(argv[3]);
    F = atoi(argv[4]);
    if (M < 1 || N < 1 || P < 1 || F < 1 || M < N) {
        fprintf(stderr, "Error | incorrect argument\n");
        exit(EXIT_FAILURE);
    }

    // Initialize random seed
    srand(time(NULL));

    // Set up signal handler for SIGINT
    signal(SIGINT, signal_handler);

    // Allocate memory for service times array
    service_times = malloc(F * sizeof(int));

    // Generate service times and print them
    printf(">>> SERVICES: <<<\n");
    for (int i = 0; i < F; i++) {
        service_times[i] = 1000 + random() % 4000;
        printf("id: %d, time: %fs\n", i + 1, service_times[i] / 1000.0);
    }
    printf("\n");

    // Create semaphore and shared memory
    key = ftok("main.c", 1 + rand() % 255);
    semDesc = semget(key, 3, IPC_CREAT | 0666);
    if (semDesc == -1) {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    shmDesc = shmget(key, (P + 1) * sizeof(int), IPC_CREAT | 0666);
    if (shmDesc == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Initialize semaphores
    semctl(semDesc, CHAIR_SEM_ID, SETVAL, N);
    semctl(semDesc, WAITING_SEM_ID, SETVAL, 0);
    semctl(semDesc, TIMES_SEM_ID, SETVAL, 1);
    sem_buf.sem_flg = 0;

    // Initialize shared memory
    int* times = (int*)shmat(shmDesc, NULL, 0);
    memset(times, -1, (P + 1) * sizeof(int));
    times[0] = 0;
    shmdt(times);

    // Create child processes for hairdressers
    for (int i = 0; i < M; i++) {
        if (fork() == 0) {
            // Set up signal handler for SIGINT
            signal(SIGINT, hairdresser_sig_handler);

            // Enter hairdresser loop
            hairdresser_loop(i);

            // Exit child process
            exit(EXIT_SUCCESS);
        }
    }

    // Enter client loop
    client_loop();

    // Wait for all child processes to exit
    while (wait(NULL) > 0);

    // Clean up resources
    free(service_times);
    semctl(semDesc, 0, IPC_RMID);
    shmdt(times);
    shmctl(shmDesc, IPC_RMID, NULL);

    // Exit main process
    exit(EXIT_SUCCESS);
}
