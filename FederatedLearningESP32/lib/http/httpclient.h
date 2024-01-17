#ifndef _httpclient
#define _httpclient

#include "esp_http_client.h"
#include "federatedlearning.h"

#define GET_GLOBAL_MODEL "http://192.168.15.100:8888/api/getglobalmodel"

FederatedLearning* getglobalmodel();

#endif