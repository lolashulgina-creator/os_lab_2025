#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#define SADDR struct sockaddr
#define SLEN sizeof(struct sockaddr_in)

int main(int argc, char **argv) {
    int port = -1;
    int bufsize = 1024;
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
                        if (bufsize <= 0) bufsize = 1024;
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

    int sockfd, n;
    char *sendline = malloc(bufsize);
    char *recvline = malloc(bufsize + 1);
    struct sockaddr_in servaddr;

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) < 0) {
        perror("inet_pton problem");
        free(sendline);
        free(recvline);
        return 1;
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket problem");
        free(sendline);
        free(recvline);
        return 1;
    }

    printf("UDP Client ready. Enter messages (Ctrl+D to exit):\n");

    while ((n = read(0, sendline, bufsize)) > 0) {
        if (sendto(sockfd, sendline, n, 0, (SADDR *)&servaddr, SLEN) == -1) {
            perror("sendto problem");
            break;
        }

        if (recvfrom(sockfd, recvline, bufsize, 0, NULL, NULL) == -1) {
            perror("recvfrom problem");
            break;
        }

        recvline[n] = '\0';
        printf("REPLY FROM SERVER: %s\n", recvline);
    }

    free(sendline);
    free(recvline);
    close(sockfd);
    return 0;
}