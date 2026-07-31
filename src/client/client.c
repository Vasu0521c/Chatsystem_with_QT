#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include "client.h"
    
void send_msg(int server_fd, char *msg, int length) {

    write(server_fd, msg, length);
}

char* recv_msg(int client_socket) {

    char *msg = malloc(1024);
    int size  = 1023;

    memset(msg, 0, size);
    read(client_socket, msg, size);

    return msg;
}

int server_connect(char *ip, int port) {

    struct sockaddr_in client;
    client.sin_port        = htons(port);
    client.sin_family      = AF_INET;
    client.sin_addr.s_addr = inet_addr(ip);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    int length        = sizeof(client);

    int connection_status = connect(client_socket, (struct sockaddr *) &client, length) ;
    if(connection_status == 0)
        printf("Connected to server successfully\n");
    else {
        printf("connecting to server failed\n");
        return -1;
    }

    return client_socket;
}
