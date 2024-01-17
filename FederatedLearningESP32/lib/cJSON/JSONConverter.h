#ifndef _jsonconverter
#define _jsonconverter

#include "federatedlearning.h"
#include "cJSON.h"

cJSON* federatedLearningToJSON(const FederatedLearning* federatedLearning);
FederatedLearning* JSONToFederatedLearning(const cJSON* json);

#endif