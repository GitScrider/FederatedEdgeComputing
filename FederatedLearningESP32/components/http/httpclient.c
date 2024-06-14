#include "httpclient.h"
#include "esp_log.h"

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
                    //ESP_LOGE(TAG, "Failed to allocate memory for response buffer");
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
            //ESP_LOGE(TAG, "HTTP_EVENT_ERROR");
            break;

        case HTTP_EVENT_ON_FINISH:
            //ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;

        case HTTP_EVENT_DISCONNECTED:
            //ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
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
    esp_err_t err = esp_http_client_perform(client);

    // if (err == ESP_OK) {
    //    // ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
    //     //         esp_http_client_get_status_code(client),
    //      //        esp_http_client_get_content_length(client));
    // } else {
    //    // ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    // }

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cJSON *req;

////////// get and post perform //////////

// esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
// {
//     switch (evt->event_id){

//     case HTTP_EVENT_ON_DATA:

//         //printf("DATA: %.*s\n",evt->data_len, (char *)evt->data);

//         printf("JSON: %d\n",is_complete_json((char *)evt->data,evt->data_len));

//         if(is_complete_json((char *)evt->data,evt->data_len)){
//             req = cJSON_Parse((char *)evt->data);
//         }
//         else{
//             req = NULL;
//         }

//         break;

//     default:
//         break;
//     }
//     return ESP_OK;
// }

esp_err_t client_event_post_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}

// void perform_get_request(const char *url)
// {
//     esp_http_client_config_t config_get = {
//     .url = url,
//     .method = HTTP_METHOD_GET,
//     .cert_pem = NULL,
//     .event_handler = client_event_get_handler,
//     .buffer_size = 4096,
//     .skip_cert_common_name_check = true, 
//     };

//     esp_http_client_handle_t client = esp_http_client_init(&config_get);
//     esp_http_client_perform(client);
//     esp_http_client_cleanup(client);

// }

void perform_post_request(const char *url, cJSON *json_data) {
    //char *post_data = cJSON_Print(json_data);
    esp_http_client_config_t config_post = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL,
        .event_handler = client_event_post_handler};
        
    esp_http_client_handle_t client = esp_http_client_init(&config_post);

    const char *post_data = "{\"data\":2}";
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "POST request sent successfully");
    } else {
        ESP_LOGE(TAG, "Failed to send POST request, error code: %d", err);
    }

    // Setting HTTP client free
    esp_http_client_cleanup(client);

    //free(post_data); // Libera a memória alocada para a string JSON
}

/////////// /////////// ///////////
 
// FederatedLearning * getglobalmodel(){
//     perform_get_request(GET_GLOBAL_MODEL);
//     //char *json_string = cJSON_Print(req);
//     //printf("%s\n",json_string);
//     //free(json_string);
//     if(req!=NULL){
//         FederatedLearning *FederatedLearningInstance = JSONToFederatedLearning(req);
//         cJSON_Delete(req);
//         return FederatedLearningInstance;
//     }
//     return NULL;
// }

// int getglobalmodelstatus(){

//     perform_get_request(GET_GLOBAL_MODEL_STATUS);

//     cJSON *status_item = cJSON_GetObjectItem(req, "status");
//     if (status_item == NULL) {
//         printf("Error getting 'status' item.\n");
//         cJSON_Delete(req);
//         return 0;
//     }

//     if (!cJSON_IsNumber(status_item)) {
//         printf("Value of 'status' is not a number.\n");
//         cJSON_Delete(req);
//         return 0;
//     }

//     int status = status_item->valueint;
    
//     cJSON_Delete(req);

//     return status;

// }

// void getregisternode(){
//     perform_get_request(GET_REGISTER_NODE);
//     char *json_string = cJSON_Print(req);
//     printf("%s\n",json_string);
//     free(json_string);
//     cJSON_Delete(req);
// }

void postglobalmodel(){
    
    // teste();
    // FederatedLearning *FDI =  getFederatedLearningInstance();
    // PrintNeuralNetwork(FDI->neuralnetwork);
    // cJSON *json_data = federatedLearningToJSON(FDI);
    // char *json_string = cJSON_Print(json_data);
    // printf("%s\n", json_string);
    // teste();
    perform_post_request(POST_GLOBAL_MODEL,NULL);
    // cJSON_Delete(json_data);
    // free(json_string);
    // teste();

}

