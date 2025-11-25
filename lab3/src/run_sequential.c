#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        printf("Child process: Starting sequential_min_max...\n");
        execl("./sequential_min_max", "sequential_min_max", "42", "100", NULL);
        perror("execl");
        exit(1);
    } else {
        printf("Parent process: Waiting for child to finish...\n");
        int status;
        wait(&status);
        printf("Parent process: Child finished with status %d\n", WEXITSTATUS(status));
    }

    return 0;
}
