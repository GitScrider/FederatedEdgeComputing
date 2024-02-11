#include "../lib/httphandlers.h"
#include "../lib/cJSON.h"
#include "../lib/federatedlearning.h"
#include "../lib/JSONConverter.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


void handle_root_request(int client_socket){

        const char *response = "HTTP/1.1 404 Not Found\nContent-Type: text/plain\n\nFile Not Found";
        write(client_socket, response, strlen(response));

}


void handle_not_found_request(int client_socket) {
    const char *not_found_response = "HTTP/1.1 404 Not Found\nContent-Type: text/plain\n\n404 Not Found";
    write(client_socket, not_found_response, strlen(not_found_response));
}

void handle_testget_request(int client_socket){
    cJSON *json_response = cJSON_CreateObject();
    cJSON_AddStringToObject(json_response, "message", "This is a JSON response for /api/testget.");
    
    char *response_str = cJSON_Print(json_response);
    cJSON_Delete(json_response);

    const char *header = "HTTP/1.1 200 OK\nContent-Type: application/json\n\n";
    write(client_socket, header, strlen(header));
    write(client_socket, response_str, strlen(response_str));
    
    free(response_str);
}


void handle_testpost_request(int client_socket, const char *request_body){
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


void handle_get_globalmodel(int client_socket) {
    // Obtém a instância do FederatedLearning
    FederatedLearning *FederatedLearningInstance = getFederatedLearningInstance();

    // Converte o NeuralNetwork para JSON
    cJSON *json_response = federatedLearningToJSON(FederatedLearningInstance);

    // Converte o JSON para uma string
    char *response_str = cJSON_Print(json_response);
    //printf("%s\n", response_str);

    // Libera a memória utilizada pelo JSON
    cJSON_Delete(json_response);

    // Prepara o cabeçalho da resposta HTTP
    const char *header = "HTTP/1.1 200 OK\nContent-Type: application/json\n\n";

    // Envia o cabeçalho para o cliente
    write(client_socket, header, strlen(header));

    // Envia a resposta JSON para o cliente
    write(client_socket, response_str, strlen(response_str));

    // Libera a memória utilizada pela string da resposta JSON
    free(response_str);
}


void handle_post_globalmodel(int client_socket, const char *request_body) {
    // Parseie o corpo JSON usando cJSON
    cJSON *jsonModel = cJSON_Parse(request_body);
    char* jsonString = cJSON_Print(jsonModel);
    //printf("%s\n", jsonString);


    if (jsonModel != NULL) {

        FederatedLearning *FederatedLearningInstance = JSONToFederatedLearning(jsonModel);
         
        // Libere a memória alocada
        cJSON_Delete(jsonModel);
    }

    // Responder à solicitação
    const char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nPOST request to /api/postglobalmodel";
    write(client_socket, response, strlen(response));
}


void handle_get_checkmodelstatus(int client_socket){
    FederatedLearning *FederatedLearningInstance = getFederatedLearningInstance();
    printf("STATUS: %d\n",FederatedLearningInstance->globalmodelstatus);
    // Criar um objeto cJSON
    cJSON *json_response = cJSON_CreateObject();


    // Adicionar a string ao objeto cJSON
    cJSON_AddItemToObject(json_response, "status", cJSON_CreateNumber(FederatedLearningInstance->globalmodelstatus));

    
    char *response_str = cJSON_Print(json_response);
    cJSON_Delete(json_response);

    const char *header = "HTTP/1.1 200 OK\nContent-Type: application/json\n\n";
    write(client_socket, header, strlen(header));
    write(client_socket, response_str, strlen(response_str));
    
    free(response_str);

}