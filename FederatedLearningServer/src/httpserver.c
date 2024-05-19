#include "../lib/httpserver.h"
#include "../lib/httphandlers.h"
#include "../lib/cJSON.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 4096

struct ThreadArgs {
    int port;
};


void handle_request(int client_socket,struct sockaddr_in client_address) {
    
    char buffer[BUFFER_SIZE] = {0};
    read(client_socket, buffer, BUFFER_SIZE - 1);
    //printf("Request received:\n%s\n", buffer);
    //Extracting the bory from the request.
    char *body_start = strstr(buffer, "\r\n\r\n");
    const char *request_body = (body_start != NULL) ? body_start + 4 : "";
    
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_address.sin_addr), client_ip, INET_ADDRSTRLEN);

    //Processing the request. Redirecting to the endpoints handlers.
    if (strstr(buffer, "GET /api/checkglobalmodel HTTP/1.1") != NULL){
        handle_get_checkmodelstatus(client_socket,client_ip);
    }
    else if(strstr(buffer, "GET /api/noderegister HTTP/1.1") != NULL){

        handle_get_noderegister(client_socket,client_ip);
    }
    else if (strstr(buffer, "GET /api/getglobalmodel HTTP/1.1") != NULL){
        handle_get_globalmodel(client_socket);
    }
    else if (strstr(buffer, "POST /api/postglobalmodel HTTP/1.1") != NULL){
        
        handle_post_globalmodel(client_socket, request_body);
    }
    else if (strstr(buffer, "POST /api/testpost") != NULL) {
        handle_testpost_request(client_socket, request_body);
    }
    else if (strstr(buffer, "GET /api/testget HTTP/1.1") != NULL){
        handle_testget_request(client_socket);
    }
    else if (strstr(buffer, "GET / HTTP/1.1") != NULL){
        handle_root_request(client_socket);
    } 
    else {
        handle_not_found_request(client_socket);
    }

    close(client_socket);
}

void *start_httpserver(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(client_addr);

    //Creating the socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error to run the socket");
        exit(EXIT_FAILURE);
    }

    //Setting the serve address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    server_addr.sin_port = htons((int)threadArgs->port);

    //Associating the socket to the server address
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error to associate the socket to the server address");
        exit(EXIT_FAILURE);
    }

    //Set the socket in listening mode
    if (listen(server_socket, 10) == -1) {
        perror("Error to set the socket in listening mode");
        exit(EXIT_FAILURE);
    }

    printf("HTTP Server running in http://%s:%d/\n", IP_ADDRESS,(int)threadArgs->port);

    while (1) {
        // Waiting for some conection
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size)) == -1) {
            perror("Erro ao aceitar a conexão");
            continue;
        }

        // Handles the customer's request
        handle_request(client_socket,client_addr);
    }

    // Close the server socket
    close(server_socket);
}
