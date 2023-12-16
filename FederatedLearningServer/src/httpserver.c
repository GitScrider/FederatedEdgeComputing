#include "../lib/httpserver.h"
#include "../lib/handlers.h"
#include "../lib/cJSON.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

struct ThreadArgs {
    int port;
};


void handle_request(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    read(client_socket, buffer, BUFFER_SIZE - 1);
    printf("Request received:\n%s\n", buffer);

    // Extraindo o corpo da solicitação (se houver).
    char *body_start = strstr(buffer, "\r\n\r\n");
    const char *request_body = (body_start != NULL) ? body_start + 4 : "";

    // Aqui você pode processar a solicitação e gerar a resposta adequada.
    // Neste exemplo, apenas redirecionamos para os handlers de endpoints.
    if(strstr(buffer, "GET /") != NULL){
        handle_root_request(client_socket);
    }    
    else if (strstr(buffer, "POST /api/testpost") != NULL) {
        handle_testpost_request(client_socket, request_body);
    }
    else if (strstr(buffer, "GET /api/testget") != NULL){
        handle_testget_request(client_socket);
    }
    else {
        handle_default_request(client_socket);
    }
    

    close(client_socket);
}

void *start_httpserver(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);

    // Criação do socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Erro ao criar o socket");
        exit(EXIT_FAILURE);
    }

    // Configuração do endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons((int)threadArgs->port);

    // Associação do socket ao endereço do servidor
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erro ao associar o socket ao endereço");
        exit(EXIT_FAILURE);
    }

    // Coloca o socket em modo de escuta
    if (listen(server_socket, 10) == -1) {
        perror("Erro ao colocar o socket em modo de escuta");
        exit(EXIT_FAILURE);
    }

    printf("Servidor HTTP rodando em http://127.0.0.1:%d/\n", (int)threadArgs->port);

    while (1) {
        // Aguarda por uma conexão
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_size)) == -1) {
            perror("Erro ao aceitar a conexão");
            continue;
        }

        // Lida com a solicitação do cliente
        handle_request(client_socket);
    }

    // Fechar o socket do servidor
    close(server_socket);
}