#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>

#define SADDR struct sockaddr

int main(int argc, char *argv[]) {
    int port = -1;
    int bufsize = 100;
    int backlog = 5;

    static struct option options[] = {
        {"port", required_argument, 0, 0},
        {"bufsize", required_argument, 0, 0},
        {"backlog", required_argument, 0, 0},
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
                        backlog = atoi(optarg);
                        if (backlog <= 0) backlog = 5;
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

    if (port == -1) {
        printf("Usage: %s --port <port> [--bufsize <size>] [--backlog <num>]\n", argv[0]);
        return 1;
    }

    const size_t kSize = sizeof(struct sockaddr_in);

    int lfd, cfd;
    int nread;
    char *buf = malloc(bufsize);
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    if ((lfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        free(buf);
        return 1;
    }

    memset(&servaddr, 0, kSize);
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(lfd, (SADDR *)&servaddr, kSize) < 0) {
        perror("bind");
        free(buf);
        close(lfd);
        return 1;
    }

    if (listen(lfd, backlog) < 0) {
        perror("listen");
        free(buf);
        close(lfd);
        return 1;
    }

    printf("TCP Server listening on port %d\n", port);

    while (1) {
        unsigned int clilen = kSize;

        if ((cfd = accept(lfd, (SADDR *)&cliaddr, &clilen)) < 0) {
            perror("accept");
            continue;
        }
        printf("Connection established\n");

        while ((nread = read(cfd, buf, bufsize)) > 0) {
            write(1, buf, nread);
        }

        if (nread == -1) {
            perror("read");
        }
        close(cfd);
        printf("Connection closed\n");
    }

    free(buf);
    close(lfd);
    return 0;
}