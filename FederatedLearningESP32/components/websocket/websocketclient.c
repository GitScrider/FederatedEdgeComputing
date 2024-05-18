#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_event.h"

#include "esp_websocket_client.h"

#include "websocketclient.h"
#include "cJSON.h"
#include "JSONConverter.h"
#include "federatedlearning.h"

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

void websocket_send_local_model(){

    // Define the websocket connection
    esp_websocket_client_config_t websocket_cfg = {};
    websocket_cfg.uri = WEBSOCKET_SERVER;
    ESP_LOGI(TAG, "Connecting to %s ...", websocket_cfg.uri);

    // Connect to Websocket Server
    esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);


    esp_websocket_client_start(client);

    char *json_string = cJSON_Print(federatedLearningToJSON(getFederatedLearningInstance()));

    if (esp_websocket_client_is_connected(client)) {
        esp_websocket_client_send_text(client, json_string, strlen(json_string), portMAX_DELAY);
    }
    free(json_string);

    // Stop websocket connection
    esp_websocket_client_stop(client);
    ESP_LOGI(TAG, "Websocket Stopped");
    esp_websocket_client_destroy(client);
}