#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "utils.h"

struct Server {
    char ip[255];
    int port;
};

struct ThreadArgs {
    struct Server server;
    uint64_t begin;
    uint64_t end;
    uint64_t mod;
    uint64_t result;
};

void* ConnectToServer(void* args) {
    struct ThreadArgs* thread_args = (struct ThreadArgs*)args;
    
    struct hostent *hostname = gethostbyname(thread_args->server.ip);
    if (hostname == NULL) {
        fprintf(stderr, "gethostbyname failed with %s\n", thread_args->server.ip);
        thread_args->result = 0;
        return NULL;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(thread_args->server.port);
    server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

    int sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) {
        fprintf(stderr, "Socket creation failed!\n");
        thread_args->result = 0;
        return NULL;
    }

    if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
        fprintf(stderr, "Connection failed to %s:%d\n", thread_args->server.ip, thread_args->server.port);
        close(sck);
        thread_args->result = 0;
        return NULL;
    }

    char task[sizeof(uint64_t) * 3];
    memcpy(task, &thread_args->begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &thread_args->end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &thread_args->mod, sizeof(uint64_t));

    if (send(sck, task, sizeof(task), 0) < 0) {
        fprintf(stderr, "Send failed\n");
        close(sck);
        thread_args->result = 0;
        return NULL;
    }

    char response[sizeof(uint64_t)];
    if (recv(sck, response, sizeof(response), 0) < 0) {
        fprintf(stderr, "Receive failed\n");
        close(sck);
        thread_args->result = 0;
        return NULL;
    }

    memcpy(&thread_args->result, response, sizeof(uint64_t));
    close(sck);
    
    return NULL;
}

int main(int argc, char **argv) {
    uint64_t k = -1;
    uint64_t mod = -1;
    char servers_file[255] = {'\0'};

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {{"k", required_argument, 0, 0},
                                          {"mod", required_argument, 0, 0},
                                          {"servers", required_argument, 0, 0},
                                          {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index);

        if (c == -1)
            break;

        switch (c) {
        case 0: {
            switch (option_index) {
            case 0:
                ConvertStringToUI64(optarg, &k);
                break;
            case 1:
                ConvertStringToUI64(optarg, &mod);
                break;
            case 2:
                memcpy(servers_file, optarg, strlen(optarg));
                break;
            default:
                printf("Index %d is out of options\n", option_index);
            }
        } break;

        case '?':
            printf("Arguments error\n");
            break;
        default:
            fprintf(stderr, "getopt returned character code 0%o?\n", c);
        }
    }

    if (k == -1 || mod == -1 || !strlen(servers_file)) {
        fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
                argv[0]);
        return 1;
    }

    // Чтение серверов из файла
    FILE* file = fopen(servers_file, "r");
    if (!file) {
        fprintf(stderr, "Cannot open servers file: %s\n", servers_file);
        return 1;
    }

    struct Server* servers = NULL;
    int servers_num = 0;
    char line[255];
    
    while (fgets(line, sizeof(line), file)) {
        char* colon = strchr(line, ':');
        if (!colon) continue;
        
        *colon = '\0';
        int port = atoi(colon + 1);
        
        servers = realloc(servers, (servers_num + 1) * sizeof(struct Server));
        strcpy(servers[servers_num].ip, line);
        servers[servers_num].port = port;
        servers_num++;
    }
    fclose(file);

    if (servers_num == 0) {
        fprintf(stderr, "No servers found in file\n");
        return 1;
    }

    // Распределение работы между серверами
    pthread_t threads[servers_num];
    struct ThreadArgs thread_args[servers_num];
    
    uint64_t numbers_per_server = k / servers_num;
    uint64_t remainder = k % servers_num;
    
    uint64_t current = 1;
    for (int i = 0; i < servers_num; i++) {
        thread_args[i].server = servers[i];
        thread_args[i].begin = current;
        thread_args[i].end = current + numbers_per_server - 1;
        if (i < remainder) {
            thread_args[i].end++;
        }
        thread_args[i].mod = mod;
        
        current = thread_args[i].end + 1;
        
        if (pthread_create(&threads[i], NULL, ConnectToServer, &thread_args[i])) {
            fprintf(stderr, "Error creating thread for server %s:%d\n", 
                    servers[i].ip, servers[i].port);
        }
    }

    // Ожидание завершения всех потоков
    uint64_t total_result = 1;
    for (int i = 0; i < servers_num; i++) {
        pthread_join(threads[i], NULL);
        total_result = MultModulo(total_result, thread_args[i].result, mod);
    }

    printf("Final result: %lu! mod %lu = %lu\n", k, mod, total_result);
    
    free(servers);
    return 0;
}