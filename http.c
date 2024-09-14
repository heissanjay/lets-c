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

void serve_file(int client_socket, const char *filename){
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        char* response = 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "File not found";
        send(client_socket, response, strlen(response), 0);
        return;
    }

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    rewind(file);

    char headers[128];
    sprintf(headers,
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: image/jpeg\r\n"
    "Content-Length: %d\r\n"
    "\r\n"
    ,
    file_size);

    send(client_socket, headers, strlen(headers), 0);
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
}

void handle_request(int client_socket){
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE-1, 0);
    if(bytes_received < 0){
        perror("recv");
        return;
    }

    buffer[bytes_received] = '\0';
    printf("Received Request: \n%s\n", buffer);


    // parse the url
    char *url = strstr(buffer, "GET ");
    if (url == NULL){
        char *resp =
            "HTTP/1.1 400 Bad request\r\n\r\n";
        send(client_socket, resp, strlen(resp), 0);
        return;
    }

    url += 4;
    char* url_end = strstr(url, " HTTP");
    if(url_end == NULL){
        char *resp =
            "HTTP/1.1 400 Bad request\r\n\r\n";
        send(client_socket, resp, strlen(resp), 0);
        return;
    }
    *url_end = '\0';

    if (strcmp(url, "/") == 0) {
        serve_file(client_socket, "image.jpg");
    } else {
        char* resp = 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"  // header is over
            "Page Not Found";
        send(client_socket, resp, strlen(resp), 0);
    }

    // sending data to the client
    // char *resp = 
    //     "HTTP/1.1 200 OK\r\n"
    //     "Content-Type: text/plain\r\n"
    //     "\r\n"
    //     "Hello, World!\n";

    // send(client_socket, resp, strlen(resp), 0);
    shutdown(client_socket, 1);
    return;
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

    // connection Loop
    while(1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_length);
        if (client_socket < 0){
            perror("accept");
            continue;
        }
        handle_request(client_socket);
        signal(SIGINT, cleanup);
    }
    return 0;
}