#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT 8888
#define MAX_CLIENTS 1000

typedef struct {
    struct sockaddr_in address;
    int sock;
} Peer;

// storing all clients
Peer clients[MAX_CLIENTS];
// bad practice but i dont wanna traverse clients list each time bro
int CURRENT_CLIENT_COUNT;

int new_client(struct sockaddr_in client_address, int client_sock);

void start();

int main(){
    start();
    return 0;
}


void start() {
    //boilerplate 
    int server_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t addr_len = sizeof(client_address);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // nonnegative int returned on success, ty opengroup API docs!
    if (server_socket < 0){
        fprintf(stderr, "Setting up the socket failed: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // bind
    int bind_error = bind(server_socket, (struct sockaddr_in*)&server_address, sizeof(server_address));
    if (bind_error != 0){
        fprintf(stderr, "Bind failed: %s\n", strerror(errno));
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) != 0){
        fprintf(stderr, "Listen failed: %s\n", strerror(errno));
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    while (1) {
        int client_socket = accept(server_socket, (struct sockaddr_in*)&client_address, &addr_len);
        if (client_socket < 0){
            fprintf(stderr, "Failed to accept new client connection: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "New client found! %s:%d\n", inet_ntoa(client_address.sin_addr),ntohs(client_address.sin_port));

        if (new_client(client_address, client_socket) != 0){
            fprintf(stdout, "Not accepting any new clients at the moment!\n");
        }

    }

    if (close(server_socket) != 0){
        fprintf(stderr, "FAILED TO CLOSE SERVER SOCKET: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}

int new_client(struct sockaddr_in client_address, int client_sock){
    if (CURRENT_CLIENT_COUNT < MAX_CLIENTS){
        clients[CURRENT_CLIENT_COUNT].address = client_address;
        clients[CURRENT_CLIENT_COUNT].sock = client_sock;
        return 0;
    }
    return -1;
}