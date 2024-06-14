#ifndef _httpclient
#define _httpclient

#include "esp_http_client.h"
#include "federatedlearning.h"
#include "JSONConverter.h"

#define GET_GLOBAL_MODEL "http://192.168.15.100:8888/api/getglobalmodel"
#define GET_GLOBAL_MODEL_STATUS "http://192.168.15.100:8888/api/checkglobalmodel"
#define GET_REGISTER_NODE "http://192.168.15.100:8888/api/noderegister"
#define POST_GLOBAL_MODEL "http://192.168.15.100:8888/api/postglobalmodel"

FederatedLearning* getglobalmodel();
int getglobalmodelstatus();
void postglobalmodel();
void getregisternode();
#endif