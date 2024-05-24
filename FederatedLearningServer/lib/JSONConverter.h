#ifndef HTTPHANDLERS_H
#define HTTPHANDLERS_H

#include "federatedlearning.h"
#include "../lib/cJSON.h"

cJSON* FederatedLearningToJSON(FederatedLearning* federatedLearning);
FederatedLearning* JSONToFederatedLearning(const cJSON* json);

#endif