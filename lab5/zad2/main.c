#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

double f(double x) {
    return 4.0 / (x * x + 1);
}

double calculate_integral(double a, double b, double dx) {
    double sum = 0.0;
    double x;
    for (x = a; x < b; x += dx) {
        sum += f(x) * dx;
    }
    return sum;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Wrong number of arguments!\n");
		exit(EXIT_FAILURE);
    }
    double dx = atof(argv[1]);
    int n = atoi(argv[2]);
    double a = 0.0;
    double b = 1.0;
    int i;
    int pipe_fd[2*n][2];
    double result = 0.0;
    pid_t pid;
    clock_t start_time, end_time;
    char report_filename[50];
    sprintf(report_filename, "report_dx=%s_n=%s.txt", argv[1], argv[2]);
    FILE *report_file = fopen(report_filename, "w");
    for (i = 0; i < n; i++) {
        pipe(pipe_fd[i]);
        pid = fork();
        if (pid == 0) {
            close(pipe_fd[i][0]);
            double x0 = a + i * (b - a) / n;
            double x1 = a + (i + 1) * (b - a) / n;
            double result_part = calculate_integral(x0, x1, dx);
            write(pipe_fd[i][1], &result_part, sizeof(double));
            close(pipe_fd[i][1]);
            exit(0);
        }
        else {
            close(pipe_fd[i][1]);
        }
    }
    start_time = clock();
    for (i = 0; i < n; i++) {
        double result_part;
        read(pipe_fd[i][0], &result_part, sizeof(double));
        result += result_part;
        close(pipe_fd[i][0]);
    }
    end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    fprintf(report_file, "n=%d, dx=%s, result=%f, time=%f\n", n, argv[1], result, elapsed_time);
    printf("n=%d, dx=%s, result=%f, time=%f\n", n, argv[1], result, elapsed_time);
    fclose(report_file);
    return 0;
}
