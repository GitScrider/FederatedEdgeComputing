#include "espconfiguration.h"



static void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{

    printf("\n\n\n\n%ld\n\n\n\n",event_id);


    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        printf("WiFi connecting ... \n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        printf("WiFi connected ... \n");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        printf("WiFi lost connection ... \n");
        break;
    case IP_EVENT_STA_GOT_IP:
        printf("WiFi got IP ... \n\n");
        break;
    default:
        //printf("ERROR ... \n\n");

        break;
    }
}

void WIFIConfiguration()
{

    nvs_flash_init();

    printf("Iniciando NVS Flash...\n");
    esp_err_t nvs_ret = nvs_flash_init();

    if (nvs_ret == ESP_OK) {
        printf("NVS Flash iniciado com sucesso!\n");
    } else {
        printf("Erro ao iniciar NVS Flash. Código de erro: %d\n", nvs_ret);
    }

    printf("\n\n\n\nTESTE2\n\n\n\n");
    // 1 - Wi-Fi/LwIP Init Phase
    esp_netif_init();                    // TCP/IP initiation 					s1.1
    esp_event_loop_create_default();     // event loop 			                s1.2
    esp_netif_create_default_wifi_sta(); // WiFi station 	                    s1.3
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_initiation); // 					                    s1.4
    // 2 - Wi-Fi Configuration Phase
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
    printf("\n\n\n\nTESTE3\n\n\n\n");
    esp_wifi_set_mode(WIFI_MODE_STA);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD}};
    printf("\n\n\n\nTESTE4\n\n\n\n");
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    // 3 - Wi-Fi Start Phase
    esp_wifi_start();
    // 4- Wi-Fi Connect Phase
    esp_wifi_connect();

    printf("WIFI was initiated n\n");

}

void UARTConfiguration(){
        
    uart_config_t uart_config = {
        .baud_rate = 115200,   
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_driver_install(UART_NUM_0, 1024, 0, 0, NULL, 0);
}

void GPIOConfiguration(){
    gpio_config_t io_conf;

    // Configurar a porta GPIO12 como saída (LED_PIN_ERROR)
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED_PIN_ERROR);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    // Configurar a porta GPIO14 como saída (LED_PIN_WORKING)
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED_PIN_WORKING);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    // Configurar a porta GPIO27 como saída (LED_PIN_SYNC)
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED_PIN_SYNC);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}
