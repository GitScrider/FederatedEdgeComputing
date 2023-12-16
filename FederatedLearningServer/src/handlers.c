#include "../lib/handlers.h"
#include "../lib/cJSON.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>




void handle_root_request(int client_socket) {
    // Caminho para o arquivo HTML
    const char *file_path = "../static/index.html";
    printf("Tentando abrir o arquivo: %s\n", file_path);

    // Abre o arquivo em modo de leitura
    FILE *file = fopen("/../static/index.html", "r");

    if (file != NULL) {
        // Lê o conteúdo do arquivo
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        // Aloca um buffer para armazenar o conteúdo do arquivo
        char *file_content = (char *)malloc(file_size + 1);
        fread(file_content, 1, file_size, file);
        fclose(file);

        // Adiciona um caractere nulo ao final do conteúdo para formar uma string válida
        file_content[file_size] = '\0';

        // Cria uma resposta HTTP com o conteúdo do arquivo HTML
        const char *response_format = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: %ld\n\n%s";
        char response[strlen(response_format) + file_size + 1];
        sprintf(response, response_format, file_size, file_content);

        // Envia a resposta ao cliente
        write(client_socket, response, strlen(response));

        // Libera a memória alocada
        free(file_content);
    } else {
        // Se o arquivo não puder ser aberto, envia uma resposta padrão
        const char *response = "HTTP/1.1 404 Not Found\nContent-Type: text/plain\n\nFile Not Found";
        write(client_socket, response, strlen(response));
    }
}

void handle_default_request(int client_socket) {
    const char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nDefault Response";
    write(client_socket, response, strlen(response));
}


void handle_testget_request(int client_socket) {
    cJSON *json_response = cJSON_CreateObject();
    cJSON_AddStringToObject(json_response, "message", "This is a JSON response for /api/testget.");
    
    char *response_str = cJSON_Print(json_response);
    cJSON_Delete(json_response);

    const char *header = "HTTP/1.1 200 OK\nContent-Type: application/json\n\n";
    write(client_socket, header, strlen(header));
    write(client_socket, response_str, strlen(response_str));
    
    free(response_str);
}


void handle_testpost_request(int client_socket, const char *request_body) {
    // Parse do corpo JSON usando cJSON
    cJSON *json = cJSON_Parse(request_body);
    if (json != NULL) {
        // Exemplo: Obter o valor da chave "key" do JSON
        cJSON *key_value = cJSON_GetObjectItem(json, "key");
        if (cJSON_IsString(key_value) && key_value->valuestring != NULL) {
            // Exemplo: Imprimir o valor da chave "key"
            printf("Value of 'key': %s\n", key_value->valuestring);
        }

        // Aqui você pode realizar outras operações com os dados JSON conforme necessário.

        // Liberar a estrutura cJSON
        cJSON_Delete(json);
    }

    // Responder à solicitação
    const char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nPOST request to /api/testpost";
    write(client_socket, response, strlen(response));
}