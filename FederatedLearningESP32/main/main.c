#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#include "espconfiguration.h"
#include "httpclient.h"
#include "websocketclient.h"
#include "federatedlearning.h"
#include "JSONConverter.h"
#include "esp_spiffs.h"


#include "driver/i2c.h"
#include "esp_system.h"


// Defina o endereço I2C do INA219 (geralmente é 0x40)
#define INA219_ADDR 0x40

// Pinos I2C
#define SDA_PIN 21
#define SCL_PIN 22

void brink_error_led(int blink){

    for(int i=0;i<blink;i++){
        gpio_set_level(LED_PIN_ERROR, 1);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN_ERROR, 0);
    }
}

void node_register(){
    gpio_set_level(LED_PIN_SYNC, 1);
    getregisternode();
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(LED_PIN_SYNC, 0);
}

int global_model_status(){
    int status=0;
    gpio_set_level(LED_PIN_SYNC, 1);
    status=getglobalmodelstatus();
    vTaskDelay(50 / portTICK_PERIOD_MS);
    gpio_set_level(LED_PIN_SYNC, 0);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    return status;
}

FederatedLearning *global_model(){
    gpio_set_level(LED_PIN_SYNC, 1);
    FederatedLearning *globalmodelinstance = getglobalmodel();
    gpio_set_level(LED_PIN_SYNC, 0);
    if(globalmodelinstance==NULL){
        while (globalmodelinstance==NULL){
            printf("Json null\n");
            brink_error_led(2);
            gpio_set_level(LED_PIN_SYNC, 1);
            globalmodelinstance = getglobalmodel();     
            gpio_set_level(LED_PIN_SYNC, 0);
        }
    }else{
            printf("Json not null\n");
        
    }
    
    return globalmodelinstance;
}

void deep_learning(){

    int ctrl=0;

    while (1){
        if(global_model_status()){
            replaceNeuralNetwork(global_model());
            NeuralNetworkTraining();
            websocket_send_local_model();
        }else{
            break;
        }
        ctrl++;
    }
}


void deep_learning_test(){
    if(global_model_status()){
        FederatedLearning *globalmodelinstance = getglobalmodel();
        replaceNeuralNetwork(globalmodelinstance);
        PrintNeuralNetwork(globalmodelinstance->neuralnetwork);
        NeuralNetworkTraining();
        //FederatedLearning *FDI = getFederatedLearningInstance();
        //PrintNeuralNetwork(FDI->neuralnetwork);
    }
}


void start_esp32_configuration(){

    UARTConfiguration();
    GPIOConfiguration();
    WIFIConfiguration();
    SPIFFSConfiguration();
    
    gpio_set_level(LED_PIN_ERROR, 0);
    gpio_set_level(LED_PIN_SYNC, 0);
    vTaskDelay(2000 / portTICK_PERIOD_MS);

}

void start_federated_learning_system_button(){
    int startled = 0;
    while (gpio_get_level(BUTTON_PIN)){
        startled = ~startled;
        gpio_set_level(LED_PIN_WORKING, startled);
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
    gpio_set_level(LED_PIN_WORKING, 1);
}

void app_main(void){
    start_esp32_configuration();
    start_federated_learning_system_button();
    node_register();
    deep_learning();
}
