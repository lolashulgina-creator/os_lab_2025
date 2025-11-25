#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void* thread1_function(void* arg) {
    printf("Поток 1: пытается захватить мьютекс 1...\n");
    pthread_mutex_lock(&mutex1);
    printf("Поток 1: захватил мьютекс 1\n");
    
    // Имитация работы
    sleep(1);
    
    printf("Поток 1: пытается захватить мьютекс 2...\n");
    pthread_mutex_lock(&mutex2);
    printf("Поток 1: захватил мьютекс 2\n");
    
    // Критическая секция
    printf("Поток 1: работает в критической секции\n");
    
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    
    return NULL;
}

void* thread2_function(void* arg) {
    printf("Поток 2: пытается захватить мьютекс 2...\n");
    pthread_mutex_lock(&mutex2);
    printf("Поток 2: захватил мьютекс 2\n");
    
    // Имитация работы
    sleep(1);
    
    printf("Поток 2: пытается захватить мьютекс 1...\n");
    pthread_mutex_lock(&mutex1);
    printf("Поток 2: захватил мьютекс 1\n");
    
    // Критическая секция
    printf("Поток 2: работает в критической секции\n");
    
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    
    printf("=== Демонстрация Deadlock ===\n");
    printf("Потоки будут заблокированы вечно!\n");
    printf("Для выхода нажми Ctrl+C\n\n");
    
    pthread_create(&thread1, NULL, thread1_function, NULL);
    pthread_create(&thread2, NULL, thread2_function, NULL);
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    printf("Эта строка никогда не выполнится из-за deadlock!\n");
    
    return 0;
}