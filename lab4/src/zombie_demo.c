#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("=== Демонстрация зомби-процессов ===\n");
    printf("Родительский процесс: PID = %d\n", getpid());
    
    // Создаем дочерний процесс
    pid_t child_pid = fork();
    
    if (child_pid == -1) {
        perror("fork");
        return 1;
    }
    
    if (child_pid == 0) {
        // Дочерний процесс
        printf("Дочерний процесс: PID = %d, PPID = %d\n", getpid(), getppid());
        printf("Дочерний процесс завершается...\n");
        exit(0); // Завершаем дочерний процесс
    } else {
        // Родительский процесс
        printf("Родительский процесс создал дочерний с PID = %d\n", child_pid);
        printf("Родительский процесс НЕ вызывает wait() - создается зомби!\n");
        
        // Ждем 10 секунд, чтобы можно было увидеть зомби в системе
        printf("Ожидание 10 секунд... (можно проверить зомби командой 'ps aux | grep defunct')\n");
        sleep(10);
        
        // Теперь ждем дочерний процесс, чтобы убрать зомби
        printf("Теперь вызываем wait() чтобы убрать зомби...\n");
        wait(NULL);
        printf("Зомби процесс убран!\n");
    }
    
    return 0;
}