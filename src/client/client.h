#ifndef CLIENT_H_
#define CLIENT_H_

int server_connect(char *ip, int port);
void send_msg(int server_fd, char *msg, int length);
char* recv_msg(int client_socket);

#endif // CLIENT_H_
