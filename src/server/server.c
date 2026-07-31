#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "vector.h"
#include "server.h"

#define MAX_MSG_SIZE 1025

void broadcast(vectar *clients, int receipent_fd, char *msg, int size) {

    int *fd    = clients -> data;
    int length = clients -> length;

    for(; length--; fd++) {
        if(*fd == receipent_fd) {
            continue;
        }
        write(*fd, msg, size);
    }
}

void* server_work(void *fd) {

    int server_fd   = *(int *)fd;
    vectar *clients = create_vectar();

    int epfd = epoll_create1(0);
    if(epfd == -1) {
        printf("epoll instance creation failed\n");
        exit(1);
    }

    struct epoll_event ev, events[10];
    ev.events  = EPOLLIN;
    ev.data.fd = server_fd;

    int epoll_ctl_status = epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);
    if(epoll_ctl_status != 0) {
        printf("epoll ctl failed\n");
        exit(1);
    }

    char buffer[MAX_MSG_SIZE];

    while (1)
    {
        int n = epoll_wait(epfd, events, 10, -1);
        if(n == -1) {
            printf("epoll waiting sequence failed\n");
            server_stop(server_fd);
            free(fd);
            exit(1);
        }

        memset(buffer, '\0', 1024);

        for(int i = 0; i < n; i++) {
            int fd = events[i].data.fd;
            if(fd == server_fd) {
                int client = accept(server_fd, NULL, NULL);
                if(client == -1) {
                    printf("connection acceptance failed\n");
                    continue;
                }

                clients = push(clients, client);
                printf("connection acceptanced\n");

                ev.events  = EPOLLIN;
                ev.data.fd = client;

                int ctl_status = epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev);
                if(ctl_status != 0) {
                    printf("epol control add sequence failed\n");
                    continue;
                }

                printf("epol control add sequence successfull\n");
                printf("Client %d connected\n", client);
            }
            else {
                int bytes = read(fd, buffer, 1023);

                char hell[1000];
                strcpy(hell, buffer);

                if(bytes <= 0) {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    printf("Client %d disconnected\n", fd);
                }
                else {
                    snprintf(buffer, 1023, "client %d says %s", fd, hell);

                    broadcast(clients, fd, buffer, strlen(buffer));
                    break;
                }
            }
        }
    }
    free(fd);
    return 0;
}

int server_start(int port) {

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family      = AF_INET;
    server.sin_port        = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    int binding_status     = bind(server_fd, (struct sockaddr *)&server, sizeof(server));
    if(binding_status == -1) {
        printf("binding failed, errno : %d\n", errno);
        return -1;
    }

    int listen_status = listen(server_fd, 10);
    if(listen_status != 0) {
        printf("listening failed\n");
        return -1;
    }

    printf("listening on\n");

    int *temp = malloc(sizeof(int));
    *temp     = server_fd;

    pthread_t server_work_thread;
    pthread_create(&server_work_thread, NULL, server_work, temp);

    return server_fd;
}

int server_stop(int server_fd) {

    close(server_fd);
    printf("server %d is shutdown\n", server_fd);
    return 0;
}
