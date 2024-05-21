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
    
    printf("post chegou ao menos\n");
    
    // Parseie o corpo JSON usando cJSON

    if(request_body!=NULL){
        printf("TEST: %s\n",request_body);
    }

     cJSON *jsonModel = cJSON_Parse(request_body);
    //  char* jsonString = cJSON_Print(jsonModel);
    //  printf("%s\n", jsonString);

    if (jsonModel != NULL) {

        FederatedLearning *FederatedLearningInstance = JSONToFederatedLearning(jsonModel);
         
        // Libere a memória alocada
        
    }else{
    printf("CONTEUDO NULO\n");
    }

    cJSON_Delete(jsonModel);
    // Responder à solicitação
    const char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nPOST request to /api/postglobalmodel";
    write(client_socket, response, strlen(response));
}

void handle_get_checkmodelstatus(int client_socket,char *ip_addr){
    FederatedLearning *FederatedLearningInstance = getFederatedLearningInstance();
    char *response_str = "{\"status\":0}";

    if(FederatedLearningInstance->nodecontrol->currentinteraction!=FederatedLearningInstance->nodecontrol->interactioncycle){
        if(FederatedLearningInstance->nodecontrol->clientnodesregistered==FederatedLearningInstance->nodecontrol->clientnodes){
            ClientNode *currentclientnode =  FederatedLearningInstance->nodecontrol->firstclientnode;
            while(currentclientnode!=NULL){
                if(strcmp(currentclientnode->ip_id,ip_addr)==0){
                    break;
                }
            }
            if(currentclientnode->interaction==FederatedLearningInstance->nodecontrol->currentinteraction
            && FederatedLearningInstance->globalmodelstatus){
                response_str = "{\"status\":1}";
            }
            // else{
            //     response_str = "{\"status\":0}";
            // }

        }
    }


    const char *header = "HTTP/1.1 200 OK\nContent-Type: application/json\n\n";
    write(client_socket, header, strlen(header));
    write(client_socket, response_str, strlen(response_str));
}

void handle_get_noderegister(int client_socket,char *ip_addr){

    printf("Client IP: %s\n", ip_addr);

    char *response_str;
    
    FederatedLearning *fedLearninginstance = getFederatedLearningInstance();
    ClientNode *clientnode;

    if(fedLearninginstance->nodecontrol->firstclientnode==NULL){
        clientnode = (ClientNode *)malloc(sizeof(ClientNode));
        clientnode->interaction=0;
        strcpy(clientnode->ip_id,ip_addr);
        clientnode->nextclientnode=NULL;
        clientnode->previousclientnode=clientnode->nextclientnode;

        fedLearninginstance->nodecontrol->firstclientnode = clientnode;
        fedLearninginstance->nodecontrol->lastclientnode = clientnode;
        
        fedLearninginstance->nodecontrol->clientnodesregistered++;

        response_str = "{\"status\":\"added\"}";
        printf("First client node Added\n");
    }
    else{
        int ctrl=1;
        clientnode = getFederatedLearningInstance()->nodecontrol->firstclientnode;

        while(clientnode!=NULL){
        //printf("%d %s %s\n",strcmp(nodeclient->ip_id,newip_id),nodeclient->ip_id,newip_id);

            if(strcmp(clientnode->ip_id,ip_addr)==0){
                response_str = "{\"status\":\"registred\"}";
                printf("Node already Added\n",response_str);
                ctrl = 0;
            }
            clientnode = clientnode->nextclientnode;
        }

        if(ctrl!=0){
            clientnode = (ClientNode *)malloc(sizeof(ClientNode));
            clientnode->interaction=0;
            strcpy(clientnode->ip_id,ip_addr);
            clientnode->nextclientnode=NULL;

            fedLearninginstance->nodecontrol->lastclientnode->nextclientnode = clientnode;
            clientnode->previousclientnode = fedLearninginstance->nodecontrol->lastclientnode;
            fedLearninginstance->nodecontrol->lastclientnode = clientnode;
            response_str = "{\"status\":\"added\"}";
            printf("Client node Added\n");
        }
    }

    if(fedLearninginstance->nodecontrol->clientnodesregistered==fedLearninginstance->nodecontrol->clientnodes){
        fedLearninginstance->globalmodelstatus=1;
    }
    printf("Global Model Status %d\n", fedLearninginstance->globalmodelstatus);

    const char *header = "HTTP/1.1 200 OK\nContent-Type: application/json\n\n";
    write(client_socket, header, strlen(header));
    write(client_socket, response_str, strlen(response_str));
}