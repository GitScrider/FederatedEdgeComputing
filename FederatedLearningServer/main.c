#include "lib/httpserver.h"
#include "lib/websocketserver.h"
#include "lib/federatedlearning.h"
#include "lib/JSONConverter.h"
#include "lib/cJSON.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

struct ThreadArgs {
    int port;
    // Adicione outros argumentos conforme necessário
};


int main() {

    FederatedLearning *FDI =  getFederatedLearningInstance();
    setFederatedLearningGlobalModel();
    //PerformanceMetrics(30,0.5);

    //PrintNeuralNeuralNetwork(FDI->neuralnetwork);

    cJSON *jsondata = federatedLearningToJSON(FDI);

    //Imprimir o JSON resultante
    char* jsonString = cJSON_Print(jsondata);
    printf("%s\n", jsonString);

    //Liberar a memória alocada
    cJSON_Delete(jsondata);
    free(jsonString);

    // Inicializar o servidor HTTP em uma thread
    pthread_t http_thread;
    struct ThreadArgs threadArgsHTTP = {8888};  // Preencha os argumentos conforme necessário
    pthread_create(&http_thread, NULL, start_httpserver, (void *)&threadArgsHTTP);
   

    // Inicializar o servidor WebSocket em outra thread
    pthread_t websocket_thread;
    struct ThreadArgs threadArgsWebSocket = {8080};  // Preencha os argumentos conforme necessário
    pthread_create(&websocket_thread, NULL, start_websocketserver, (void *)&threadArgsWebSocket);


    // Aguardar as threads terminarem (não atingido neste exemplo)
    pthread_join(websocket_thread, NULL);
    pthread_join(http_thread, NULL);

    return 0;

}
