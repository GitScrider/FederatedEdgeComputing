#include "../lib/websockethandlers.h"
#include "../lib/federatedlearning.h"
#include "../lib/JSONConverter.h"
#include <string.h>

void handle_clint_model_message(const char * message, int length){
    
    char* json_string;
    json_string =strndup(message, length);

    printf("\n\nTESTE 1\n\n");
    if (json_string != NULL) {
        cJSON *json_model = cJSON_Parse(json_string);
        printf("\n\nTESTE 2\n\n");
        if (json_model != NULL) { 
            printf("\n\nTESTE 3\n\n");
            FederatedLearning *clientmodel = JSONToFederatedLearning(json_model);
            if(clientmodel != NULL){
                printf("\n\nTESTE 4\n\n");
                AggregationModel(clientmodel);
            }
                cJSON_Delete(json_model);
            }

            free(json_string);
        }


    
}
