#include "httpclient.h"
#include "esp_log.h"

#define TAG "HTTP_CLIENT"

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


// Estrutura para armazenar os dados da requisição
cJSON *req;

// Função de manipulador de eventos para requisições GET
esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id){

    case HTTP_EVENT_ON_DATA:

        //printf("DATA: %.*s\n",evt->data_len, (char *)evt->data);
        req = cJSON_Parse((char *)evt->data);

        if (req == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL) {
                fprintf(stderr, "Erro durante a análise JSON: %s\n", error_ptr);
            }
            // Lide com o erro ou retorne
            printf("json nulo\n");
        }
        else{
            printf("deu\n");
            char *jsonString = cJSON_Print(req);
            //printf("JSON: %s\n", jsonString);
            free(jsonString);

        }

        break;

    default:
        break;
    }
    return ESP_OK;
}


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



// Função para fazer uma requisição GET para um recurso específico e retornar os dados
void perform_get_request(const char *url)
{
    esp_http_client_config_t config_get = {
    .url = url,
    .method = HTTP_METHOD_GET,
    .cert_pem = NULL,
    .event_handler = client_event_get_handler,
    .buffer_size = 4096,
    .skip_cert_common_name_check = true, 
    };

    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);

}

void perform_post_request(const char *url, cJSON *json_data) {
    // Converte o objeto cJSON para uma string JSON
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

    // Liberando o cliente HTTP
    esp_http_client_cleanup(client);

    //free(post_data); // Libera a memória alocada para a string JSON
}

// Funções específicas para cada recurso
FederatedLearning * getglobalmodel()
{

    perform_get_request(GET_GLOBAL_MODEL);


    //printf("Data: %.*s\n", req.data_len, req.data);

    // if(is_utf8(req.data)){
    //     printf("SIM!");
    // }else{
    //     printf("NAO!");

    // }

    FederatedLearning *FederatedLearningInstance = JSONToFederatedLearning(req);


    printf("TESTE1\n");

    // char *jsonString = cJSON_Print(jsonModel);
    // printf("JSON: %s\n", jsonString);
    // free(jsonString);

    //printf("TESTE2\n");


    //FederatedLearning *FederatedLearningInstance = JSONToFederatedLearning(req);

        cJSON_Delete(req);


    return FederatedLearningInstance;
}

int getglobalmodelstatus(){

    perform_get_request(GET_GLOBAL_MODEL_STATUS);

    //printf("Data: %.*s\n", req->data_len, req->data);


    cJSON *status_item = cJSON_GetObjectItem(req, "status");
    if (status_item == NULL) {
        printf("Error getting 'status' item.\n");
        cJSON_Delete(req);
        return -1;
    }

    if (!cJSON_IsNumber(status_item)) {
        printf("Value of 'status' is not a number.\n");
        cJSON_Delete(req);
        return -1;
    }

    int status = status_item->valueint;
    
    cJSON_Delete(req);

    return status;

}

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