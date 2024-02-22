#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#include "espconfiguration.h"
#include "httpclient.h"
#include "websocketclient.h"
#include "federatedlearning.h"

#include "esp_spiffs.h"

#include "esp_websocket_client.h"


#define WEBSOCKET_SERVER "ws://192.168.15.100:8080/"

static const char *TAG = "WebSocketClient";


static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
        break;
    case WEBSOCKET_EVENT_DATA:
        ESP_LOGW(TAG, "Received=%.*s\n", data->data_len, (char *)data->data_ptr);
        break;
    }
}

static void websocket_app_start(void)
{
    // Define the websocket connection
    esp_websocket_client_config_t websocket_cfg = {};
    websocket_cfg.uri = WEBSOCKET_SERVER;
    ESP_LOGI(TAG, "Connecting to %s ...", websocket_cfg.uri);

    // Connect to Websocket Server
    esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);

    // Send "Hello ..." to Websocket Server
    esp_websocket_client_start(client);
    char data[32];
    int i = 0;
    while (i < 10) {
        if (esp_websocket_client_is_connected(client)) {
            int len = sprintf(data, "Hello %04d", i++);
            ESP_LOGI(TAG, "Sending %s", data);
            esp_websocket_client_send_text(client, data, len, portMAX_DELAY);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    // Stop websocket connection
    esp_websocket_client_stop(client);
    ESP_LOGI(TAG, "Websocket Stopped");
    esp_websocket_client_destroy(client);
}



void deeplearning(){
    FederatedLearning *test = getglobalmodel();
    printf("TESTASSO\n");
    teste();
    replaceNeuralNetwork(test);
    teste();
    FederatedLearning *FDI = getFederatedLearningInstance();
    PrintNeuralNetwork(FDI->neuralnetwork);
    teste();
    NeuralNetworkTraining(FDI->neuralnetwork, 0.01,10,120);
    teste();
    printf("%d \n",FDI->globalmodelstatus);
}

void app_main(void)
{
    UARTConfiguration();
    GPIOConfiguration();
    WIFIConfiguration();
    SPIFFSConfiguration();

    gpio_set_level(LED_PIN_ERROR, 1);
    gpio_set_level(LED_PIN_WORKING, 1);
    gpio_set_level(LED_PIN_SYNC, 1);

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    
    teste();

    //websocket_app_start();

    //deeplearning();

    //printf("%d\n",getglobalmodelstatus());

    postglobalmodel();

}
