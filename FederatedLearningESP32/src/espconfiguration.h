#ifndef _espconfiguration
#define _espconfiguration

#include "driver/gpio.h"
#include "driver/uart.h"

#include "esp_log.h"
#include "esp_wifi.h"


#include "nvs_flash.h"



#define LED_PIN_ERROR 12
#define LED_PIN_WORKING 14
#define LED_PIN_SYNC 27

#define WIFI_SSID       "56kbps2.4GHz"
#define WIFI_PASSWORD   "00000001"

void WIFIConfiguration();
void UARTConfiguration();
void GPIOConfiguration();

#endif