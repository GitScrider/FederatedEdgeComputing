#ifndef HTTPHANDLERS_H
#define HTTPHANDLERS_H



//test
void handle_testpost_request(int client_socket, const char *request_body);
void handle_testget_request(int client_socket);
//page
void handle_root_request(int client_socket);
//api
void handle_get_globalmodel(int client_socket);
void handle_post_globalmodel(int client_socket,const char *request_body);
void handle_get_checkmodelstatus(int client_socket);
void handle_not_found_request(int client_socket);

#endif
