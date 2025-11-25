#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <stdbool.h>

typedef struct {
    int start;
    int end;
    int mod;
    long long partial_result;
} ThreadArgs;

long long total_result = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* compute_partial_factorial(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    args->partial_result = 1;
    
    for (int i = args->start; i <= args->end; i++) {
        args->partial_result = (args->partial_result * i) % args->mod;
    }
    
    pthread_mutex_lock(&mutex);
    total_result = (total_result * args->partial_result) % args->mod;
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

int main(int argc, char** argv) {
    int k = 0;
    int pnum = 0;
    int mod = 0;
    
    static struct option options[] = {
        {"k", required_argument, 0, 0},
        {"pnum", required_argument, 0, 0},
        {"mod", required_argument, 0, 0},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    while (1) {
        int c = getopt_long(argc, argv, "", options, &option_index);
        if (c == -1) break;
        
        switch (c) {
            case 0:
                switch (option_index) {
                    case 0:
                        k = atoi(optarg);
                        if (k <= 0) {
                            printf("k must be positive number\n");
                            return 1;
                        }
                        break;
                    case 1:
                        pnum = atoi(optarg);
                        if (pnum <= 0) {
                            printf("pnum must be positive number\n");
                            return 1;
                        }
                        break;
                    case 2:
                        mod = atoi(optarg);
                        if (mod <= 0) {
                            printf("mod must be positive number\n");
                            return 1;
                        }
                        break;
                    default:
                        printf("Index %d is out of options\n", option_index);
                }
                break;
            case '?':
                break;
            default:
                printf("getopt returned character code 0%o?\n", c);
        }
    }
    
    if (k == 0 || pnum == 0 || mod == 0) {
        printf("Usage: %s --k \"num\" --pnum \"num\" --mod \"num\"\n", argv[0]);
        return 1;
    }
    
    pthread_t threads[pnum];
    ThreadArgs thread_args[pnum];
    
    int numbers_per_thread = k / pnum;
    int remainder = k % pnum;
    
    int current_start = 1;
    for (int i = 0; i < pnum; i++) {
        thread_args[i].start = current_start;
        thread_args[i].end = current_start + numbers_per_thread - 1;
        if (i < remainder) {
            thread_args[i].end++;
        }
        thread_args[i].mod = mod;
        
        current_start = thread_args[i].end + 1;
        
        if (pthread_create(&threads[i], NULL, compute_partial_factorial, &thread_args[i])) {
            printf("Error: pthread_create failed!\n");
            return 1;
        }
    }
    
    for (int i = 0; i < pnum; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    
    printf("%d! mod %d = %lld\n", k, mod, total_result);
    
    return 0;
}