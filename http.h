#ifndef HTTP_H
#define HTTP_H

void serve_file(int client_socket, const char *filename);
void handle_request(int client_socket);

#endif