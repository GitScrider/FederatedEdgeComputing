#include "../lib/websockethandlers.h"
#include "../lib/federatedlearning.h"
#include "../lib/JSONConverter.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void handle_clint_model_message(const char * message, int length){
    
    char* json_string;
    json_string =strndup(message, length);

    if (json_string != NULL) {
        cJSON *json_model = cJSON_Parse(json_string);
        if (json_model != NULL) { 
            FederatedLearning *clientmodel = JSONToFederatedLearning(json_model);
            if(clientmodel != NULL){
                AggregationModel(clientmodel);
            }
                cJSON_Delete(json_model);
            }

            free(json_string);
        }


    
}
