#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

#include "espconfiguration.h"
#include "httpclient.h"
#include "federatedlearning.h"

#include "esp_spiffs.h"
#include "esp_log.h"


void app_main(void)
{

    UARTConfiguration();
    GPIOConfiguration();
    gpio_set_level(LED_PIN_ERROR, 1);
    gpio_set_level(LED_PIN_WORKING, 1);
    gpio_set_level(LED_PIN_SYNC, 1);
    WIFIConfiguration();


    vTaskDelay(2000 / portTICK_PERIOD_MS);

    FederatedLearning *test = getglobalmodel();
    
    printf("TESTASSO\n");
    teste();

    replaceNeuralNetwork(test);

    teste();

    FederatedLearning *FDI = getFederatedLearningInstance();

    PrintNeuralNetwork(FDI->neuralnetwork);

    teste();



        NeuralNetworkTraining(FDI->neuralnetwork, 4, 3, 0.001,1000);




    teste();
    printf("%d \n",FDI->globalmodelstatus);



}
