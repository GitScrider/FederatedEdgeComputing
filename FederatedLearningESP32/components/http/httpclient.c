#include "httpclient.h"
#include "esp_log.h"
#include "esp_timer.h"

int is_utf8(const char *str) {
    while (*str) {
        if ((*str & 0x80) == 0) {
            // Caractere ASCII de um byte
            str++;
        } else if ((*str & 0xE0) == 0xC0) {
            // Caractere multibyte de dois bytes
            if ((str[1] & 0xC0) != 0x80)
                return 0;
            str += 2;
        } else if ((*str & 0xF0) == 0xE0) {
            // Caractere multibyte de três bytes
            if ((str[1] & 0xC0) != 0x80 || (str[2] & 0xC0) != 0x80)
                return 0;
            str += 3;
        } else if ((*str & 0xF8) == 0xF0) {
            // Caractere multibyte de quatro bytes
            if ((str[1] & 0xC0) != 0x80 || (str[2] & 0xC0) != 0x80 || (str[3] & 0xC0) != 0x80)
                return 0;
            str += 4;
        } else {
            // Byte inválido
            return 0;
        }
    }
    return 1;
}

int is_complete_json(const char *json, size_t len) {
    int count_open_braces = 0;
    int count_close_braces = 0;

    for (size_t i = 0; i < len; ++i) {
        if (json[i] == '{') {
            count_open_braces++;
        } else if (json[i] == '}') {
            count_close_braces++;
        }
    }

    return (count_open_braces == count_close_braces && count_open_braces > 0);
}


//////////////////////////////////////////////////GET//////////////////////////////////////////////////

static const char *TAG = "HTTP_CLIENT";

static char *response_buffer = NULL;
static int response_buffer_length = 0;

esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            if (evt->data_len > 0) {
                // Reallocate the buffer to hold the new data
                char *new_buf = realloc(response_buffer, response_buffer_length + evt->data_len + 1);

                if (new_buf == NULL) {
                    ESP_LOGE(TAG, "Failed to allocate memory for response buffer");
                    return ESP_FAIL;
                }
                response_buffer = new_buf;
                // Copy new data into the buffer
                memcpy(response_buffer + response_buffer_length, evt->data, evt->data_len);
                response_buffer_length += evt->data_len;
                response_buffer[response_buffer_length] = '\0'; // Null-terminate the buffer

            }
            break;

        case HTTP_EVENT_ERROR:
            ESP_LOGE(TAG, "HTTP_EVENT_ERROR");
            break;

        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;

        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;

        default:
            break;
    }
    return ESP_OK;
}


void perform_get_request(const char *url)
{
    response_buffer = NULL;
    response_buffer_length = 0;

    esp_http_client_config_t config_get = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler,
        .buffer_size = 4096,
        .skip_cert_common_name_check = true, 
    };

    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_err_t err;
    int retries = 0;

    while (retries < MAX_RETRIES) {
    
        int64_t start_time = esp_timer_get_time();

        err = esp_http_client_perform(client); // A requisição ocorre aqui

        if (err == ESP_OK) {
            
            int64_t end_time = esp_timer_get_time();
            int64_t elapsed_time = (end_time - start_time) / 1000; 

            ESP_LOGI(TAG, "Request Successful, Latency: %lld ms", elapsed_time);
            break; 

        } else {
            ESP_LOGE(TAG, "Error sending request: %s", esp_err_to_name(err));
        }

        int64_t current_time = esp_timer_get_time();
        if ((current_time - start_time) / 1000 > TIMEOUT_MS) {
            ESP_LOGW(TAG, "Timeout reached, rebroadcasting... (tries: %d)", retries + 1);
            retries++;
        }
    }

    if (retries == MAX_RETRIES) {
        ESP_LOGE(TAG, "Failed after %d tries", MAX_RETRIES);
    }

    esp_http_client_cleanup(client);
}

int getglobalmodelstatus() {
    perform_get_request(GET_GLOBAL_MODEL_STATUS);
    int status = 0; // Default status value
    if (response_buffer != NULL) {
        cJSON *req = cJSON_Parse(response_buffer);
        if (req != NULL) {
            cJSON *status_item = cJSON_GetObjectItem(req, "status");
            if (status_item != NULL && cJSON_IsNumber(status_item)) {
                status = status_item->valueint;
            } else {
                printf("Error getting 'status' item or value is not a number.\n");
            }
            cJSON_Delete(req);
        } else {
            printf("Failed to parse JSON response\n");
        }
        free(response_buffer);
        response_buffer = NULL;
    } else {
        printf("No response received or response is empty.\n");
    }
    return status;
}

void getregisternode() {
    perform_get_request(GET_REGISTER_NODE);
    if (response_buffer != NULL) {
        cJSON *req = cJSON_Parse(response_buffer);
        if (req != NULL) {
            char *json_string = cJSON_Print(req);
            if (json_string != NULL) {
                printf("%s\n", json_string);
                free(json_string);
            } else {
                printf("Failed to print JSON.\n");
            }
            cJSON_Delete(req);
        } else {
            printf("Failed to parse JSON response.\n");
        }
        free(response_buffer);
        response_buffer = NULL;
    } else {
        printf("No response received or response is empty.\n");
    }
}

FederatedLearning *getglobalmodel() {
    perform_get_request(GET_GLOBAL_MODEL);

    FederatedLearning *FederatedLearningInstance = NULL;
    if (response_buffer != NULL) {
        cJSON *req = cJSON_Parse(response_buffer);
        if (req != NULL) {
            FederatedLearningInstance = JSONToFederatedLearning(req);
            cJSON_Delete(req);
        } else {
           // ESP_LOGE(TAG, "Failed to parse JSON response");
        }
        free(response_buffer);
        response_buffer = NULL;
    }
    return FederatedLearningInstance;
}


//Not working
//////////////////////////////////////////////////POST//////////////////////////////////////////////////

// Função de callback para tratar eventos HTTP
esp_err_t _http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
                if (!esp_http_client_is_chunked_response(evt->client)) {
                    printf("%.*s", evt->data_len, (char*)evt->data);
                }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        default:
            ESP_LOGI(TAG, "Unhandled event ID: %d", evt->event_id);
            break;
    }
    return ESP_OK;
}


void http_post_task(void *pvParameters) {
    esp_err_t err;

    esp_http_client_config_t config = {
        .url = "http://192.168.15.50:8888/api/testpost",
        .event_handler = _http_event_handler,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    esp_http_client_set_method(client, HTTP_METHOD_POST);

    // Cabeçalhos adicionais
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_header(client, "Accept", "*/*");
    esp_http_client_set_header(client, "User-Agent", "ESP32");

    // Corpo da requisição
    const char *post_data = "{\"key\":\"TESTEPOST\",\"value\":1}";

    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    // Enviando a requisição POST
    err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        // ESP_LOGI(TAG, "HTTPS Status = %d, content_length = %d",
        //          esp_http_client_get_status_code(client),
        //          esp_http_client_get_content_length(client));
    } else {
        //ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    // Limpeza
    esp_http_client_cleanup(client);
    vTaskDelete(NULL);
}

