#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_THREADS 32
#define TIMEOUT 100000
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_UNDERLINE "\x1b[4m"
#define ANSI_COLOR_BOLD "\x1b[1m"
#define ANSI_COLOR_RESET "\x1b[0m"

void *scan_port(void *port_info);
int is_port_open(int port, const char *ip_address);
int is_ip_adress(const char* str);

typedef struct {
    int start_port;
    int end_port;
    const char *ip_address;
} PortRange;

int main(int argc, char *argv[])
{
    if (argc != 4 || !is_ip_adress(argv[1])) {
        printf("Usage: %s <IP_ADDRESS> <START_PORT> <END_PORT>\n", argv[0]);
        exit(1);
    }

    const char *ip_address = argv[1];
    int start_port = atoi(argv[2]);
    int end_port = atoi(argv[3]);

    PortRange port_range = {
        .start_port = start_port,
        .end_port = end_port,
        .ip_address = ip_address,
    };

    pthread_t threads[MAX_THREADS];
    int num_threads = 0;

    for (int i = start_port; i <= end_port; i++) {
        pthread_create(&threads[num_threads++], NULL, scan_port, (void *)&port_range);
        if (num_threads >= MAX_THREADS) {
            for (int j = 0; j < num_threads; j++) {
                pthread_join(threads[j], NULL);
            }
            num_threads = 0;
        }
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}

int is_ip_adress(const char* str)
{
    int num = 0;
    int dots = 0;
    int len = strlen(str);
    char* copy = strdup(str);
    char* token = strtok(copy, ".");
    
    if (len < 7 || len > 15)
        return 0;
    if (token == NULL)
        return 0;
    while (token) {
        if (dots > 3)
            return 0;
        num = atoi(token);
        if (num < 0 || num > 255)
            return 0;
        if (num == 0 && strlen(token) > 1)
            return 0;
        if (num != 0 && token[0] == '0')
            return 0;
        dots++;
        token = strtok(NULL, ".");
    }
    if (dots != 4)
        return 0;

    free(copy);
    return 1;
}

void *scan_port(void *port_info)
{
    PortRange *port_range = (PortRange *)port_info;
    int port = port_range->start_port++;

    if (is_port_open(port, port_range->ip_address)) {
        printf(ANSI_COLOR_GREEN ANSI_COLOR_UNDERLINE"\nopen"
        ANSI_COLOR_RESET ANSI_COLOR_BOLD " %d" ANSI_COLOR_RESET "/tcp\n", port);
    }

    return NULL;
}

int is_port_open(int port, const char *ip_address)
{
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip_address, &server_addr.sin_addr);

    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    int result = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (result < 0 && errno == EINPROGRESS) {
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(sockfd, &fdset);

        struct timeval timeout;
        timeout.tv_usec = TIMEOUT;

        result = select(sockfd + 1, NULL, &fdset, NULL, &timeout);
        if (result < 0) {
            perror("Error in select");
            close(sockfd);
            exit(1);
        } else if (result == 0) {
            close(sockfd);
            return 0;
        } else {
            int error = 0;
            socklen_t error_len = sizeof(error);
            getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &error_len);
            if (error != 0) {
                close(sockfd);
                return 0;
            }
        }
    }
    fcntl(sockfd, F_SETFL, flags);
    close(sockfd);
    return 1;
}
