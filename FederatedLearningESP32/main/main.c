#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#include "espconfiguration.h"
#include "httpclient.h"
#include "websocketclient.h"
//#include "websocketclient.h"
#include "federatedlearning.h"
#include "JSONConverter.h"

#include "esp_spiffs.h"

//#include "esp_websocket_client.h"





// static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
// {
//     esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
//     switch (event_id) {
//     case WEBSOCKET_EVENT_CONNECTED:
//         ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
//         break;
//     case WEBSOCKET_EVENT_DATA:
//         ESP_LOGW(TAG, "Received=%.*s\n", data->data_len, (char *)data->data_ptr);
//         break;
//     }
// }

// static void websocket_app_start(void)
// {
//     // Define the websocket connection
//     esp_websocket_client_config_t websocket_cfg = {};
//     websocket_cfg.uri = WEBSOCKET_SERVER;
//     ESP_LOGI(TAG, "Connecting to %s ...", websocket_cfg.uri);

//     // Connect to Websocket Server
//     esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
//     esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);


//     esp_websocket_client_start(client);

//     char *json_string = cJSON_Print(federatedLearningToJSON(getFederatedLearningInstance()));

//     if (esp_websocket_client_is_connected(client)) {
//         esp_websocket_client_send_text(client, json_string, strlen(json_string), portMAX_DELAY);
//     }

//     // Stop websocket connection
//     esp_websocket_client_stop(client);
//     ESP_LOGI(TAG, "Websocket Stopped");
//     esp_websocket_client_destroy(client);
// }

void deep_learning(){
    FederatedLearning *globalmodel = getglobalmodel();
    replaceNeuralNetwork(globalmodel);
    FederatedLearning *FDI = getFederatedLearningInstance();
    PrintNeuralNetwork(FDI->neuralnetwork);
    //NeuralNetworkTraining();

    //cJSON* teste= federatedLearningToJSON(FDI);

    char *json_string = cJSON_Print(federatedLearningToJSON(FDI));

    // Verificar se a string JSON foi criada com sucesso
    if (json_string == NULL) {
        fprintf(stderr, "Erro ao criar a string JSON.\n");
        return ;
    }

    // Imprimir a string JSON
    printf("%s\n", json_string);

    // Liberar a memória alocada para a string JSON
    free(json_string);
    

    websocket_send_local_model();

}

void start_federated_learning(){

    int startled = 0;

    UARTConfiguration();
    GPIOConfiguration();
    WIFIConfiguration();
    SPIFFSConfiguration();
    
    gpio_set_level(LED_PIN_ERROR, 0);
    gpio_set_level(LED_PIN_SYNC, 0);


    vTaskDelay(2000 / portTICK_PERIOD_MS);

    while (gpio_get_level(BUTTON_PIN)){
        startled = ~startled;
        gpio_set_level(LED_PIN_WORKING, startled);
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
    gpio_set_level(LED_PIN_WORKING, 1);

}

void app_main(void){
    start_federated_learning();
    deep_learning();
    //printf("%d\n",getglobalmodelstatus());
}
