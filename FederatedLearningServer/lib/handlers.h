#ifndef HANDLERS_H
#define HANDLERS_H

void handle_default_request(int client_socket);
void handle_testpost_request(int client_socket, const char *request_body);
void handle_testget_request(int client_socket);
void handle_root_request(int client_socket);

#endif
