#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>


#define PORT 8000
#define BUFFER_SIZE 1024


int server_socket;

void handle_request(int client_socket){
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE-1, 0);
    if(bytes_received < 0){
        perror("recv");
        return;
    }

    buffer[bytes_received] = '\0';
    printf("Received Request: \n%s\n", buffer);


    // sending data to the client
    char *resp = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Hello, World!\n";

    send(client_socket, resp, strlen(resp), 0);
    shutdown(client_socket, 1);
}

void cleanup(){
    shutdown(server_socket, 2);
    exit(0);
}


int main(int argc, char *argv[]) {
    int  client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_length = sizeof(client_address);
    const char *hostaddr;
    hostaddr = "127.0.0.1";

    // creating the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0){
        perror("socket");
        exit(1);
    }

    // creating address for the socket
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, hostaddr, &server_address.sin_addr);


    // binding the address to the socket.
    if ((bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0)) {
        perror("bind");
        exit(1);
    }

    // listing for the connection to the socket
    if ((listen(server_socket, 3) < 0)) {
        perror("listen");
        exit(1);
    }

    printf("Server Listening to the PORT %d\n", PORT);
    signal(SIGINT, cleanup);

    // connection Loop
    while(1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_length);
        if (client_socket < 0){
            perror("accept");
            continue;
        }
        handle_request(client_socket);
    }
    return 0;
}