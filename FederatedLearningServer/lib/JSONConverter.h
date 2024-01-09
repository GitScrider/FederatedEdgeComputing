#ifndef HTTPHANDLERS_H
#define HTTPHANDLERS_H

#include "federatedlearning.h"
#include "../lib/cJSON.h"

cJSON* federatedLearningToJSON(const FederatedLearning* federatedLearning);
FederatedLearning* JSONToFederatedLearning(const cJSON* json);

#endif