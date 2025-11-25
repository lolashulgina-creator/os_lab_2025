#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#define SADDR struct sockaddr
#define SIZE sizeof(struct sockaddr_in)

int main(int argc, char *argv[]) {
    int port = -1;
    int bufsize = 100;
    char server_ip[256] = {0};

    static struct option options[] = {
        {"port", required_argument, 0, 0},
        {"bufsize", required_argument, 0, 0},
        {"ip", required_argument, 0, 0},
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
                        port = atoi(optarg);
                        break;
                    case 1:
                        bufsize = atoi(optarg);
                        if (bufsize <= 0) bufsize = 100;
                        break;
                    case 2:
                        strncpy(server_ip, optarg, sizeof(server_ip) - 1);
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

    if (port == -1 || strlen(server_ip) == 0) {
        printf("Usage: %s --port <port> --ip <server_ip> [--bufsize <size>]\n", argv[0]);
        return 1;
    }

    int fd;
    int nread;
    char *buf = malloc(bufsize);
    struct sockaddr_in servaddr;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creating");
        free(buf);
        return 1;
    }

    memset(&servaddr, 0, SIZE);
    servaddr.sin_family = AF_INET;

    if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0) {
        perror("bad address");
        free(buf);
        close(fd);
        return 1;
    }

    servaddr.sin_port = htons(port);

    if (connect(fd, (SADDR *)&servaddr, SIZE) < 0) {
        perror("connect");
        free(buf);
        close(fd);
        return 1;
    }

    printf("Connected to server %s:%d\n", server_ip, port);
    printf("Input message to send (Ctrl+D to finish):\n");

    while ((nread = read(0, buf, bufsize)) > 0) {
        if (write(fd, buf, nread) < 0) {
            perror("write");
            free(buf);
            close(fd);
            return 1;
        }
    }

    free(buf);
    close(fd);
    return 0;
}