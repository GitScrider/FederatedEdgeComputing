#include "../lib/JSONConverter.h"
#include "../lib/federatedlearning.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//FFEDERATED LEARNING TO JSON
cJSON* FederatedLearningToJSON(FederatedLearning* federatedLearning) {
    cJSON* root = cJSON_CreateObject();

    cJSON_AddItemToObject(root, "globalmodelstatus", cJSON_CreateNumber(federatedLearning->globalmodelstatus));
    cJSON_AddItemToObject(root, "trainingscounter", cJSON_CreateNumber(federatedLearning->trainingscounter));
    
    cJSON* jsonNeuralNetwork = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "neuralnetwork", jsonNeuralNetwork);

    cJSON_AddItemToObject(jsonNeuralNetwork, "epoch", cJSON_CreateNumber(federatedLearning->neuralnetwork->epoch));
    cJSON_AddItemToObject(jsonNeuralNetwork, "alpha", cJSON_CreateNumber(federatedLearning->neuralnetwork->alpha));
    cJSON_AddItemToObject(jsonNeuralNetwork, "regularization", cJSON_CreateNumber(federatedLearning->neuralnetwork->regularization));
    cJSON_AddItemToObject(jsonNeuralNetwork, "lambda", cJSON_CreateNumber(federatedLearning->neuralnetwork->lambda));
    cJSON_AddItemToObject(jsonNeuralNetwork, "percentualtraining", cJSON_CreateNumber(federatedLearning->neuralnetwork->percentualtraining));
    cJSON_AddItemToObject(jsonNeuralNetwork, "lossfunctiontype", cJSON_CreateNumber(federatedLearning->neuralnetwork->lossfunctiontype));
    cJSON_AddItemToObject(jsonNeuralNetwork, "layers", cJSON_CreateNumber(federatedLearning->neuralnetwork->layers));

    cJSON* layersArray = cJSON_CreateArray();
    struct Layer* currentLayer = federatedLearning->neuralnetwork->firstlayer;

    while (currentLayer != NULL) {
        cJSON* layerObject = cJSON_CreateObject();
        cJSON_AddItemToObject(layerObject, "activationfunctiontype", cJSON_CreateNumber(currentLayer->activationfunctiontype));
        cJSON_AddItemToObject(layerObject, "neurons", cJSON_CreateNumber(currentLayer->neurons));

        cJSON* neuronsArray = cJSON_CreateArray();
        struct Neuron* currentNeuron = currentLayer->firstneuron;

        while (currentNeuron != NULL) {
            cJSON* neuronObject = cJSON_CreateObject();
            cJSON_AddItemToObject(neuronObject, "neurontype", cJSON_CreateString(currentNeuron->neurontype));
            cJSON_AddItemToObject(neuronObject, "weights", cJSON_CreateNumber(currentNeuron->weights));
            cJSON_AddItemToObject(neuronObject, "bias", cJSON_CreateNumber(currentNeuron->bias));

            // Adiciona um array de pesos para cada neurônio
            cJSON* weightsArray = cJSON_CreateArray();
            struct Weight* currentWeight = currentNeuron->firstweight;

            while (currentWeight != NULL) {
                cJSON_AddItemToArray(weightsArray, cJSON_CreateNumber(currentWeight->weight));
                currentWeight = currentWeight->nextweight;
            }

            cJSON_AddItemToObject(neuronObject, "weightsArray", weightsArray);

            cJSON_AddItemToArray(neuronsArray, neuronObject);
            currentNeuron = currentNeuron->nextneuron;
        }

        cJSON_AddItemToObject(layerObject, "neuronsArray", neuronsArray);
        cJSON_AddItemToArray(layersArray, layerObject);
        currentLayer = currentLayer->nextlayer;
    }

    cJSON_AddItemToObject(jsonNeuralNetwork, "layersArray", layersArray);

    // Adicione outros campos da estrutura FederatedLearning conforme necessário

    return root;
}


//JSON TO FEDERATED LEARNING  

FederatedLearning* JSONToFederatedLearning(const cJSON* json) {
    if (json == NULL || !cJSON_IsObject(json)) {
        printf("Não foi possível converter.\n");
        return NULL;
    }

    FederatedLearning* federatedLearning = (FederatedLearning*)malloc(sizeof(FederatedLearning));
    if (federatedLearning == NULL) {
        return NULL;  // Falha na alocação de memória
    }

    cJSON* statusItem = cJSON_GetObjectItem(json, "globalmodelstatus");
    if (cJSON_IsNumber(statusItem)) {
        federatedLearning->globalmodelstatus = statusItem->valueint;
    } else {
        federatedLearning->globalmodelstatus = 0;  // Valor padrão ou tratamento de erro
    }

    //printf("global model status %d\n",federatedLearning->globalmodelstatus);

    cJSON* trainingsCounterItem = cJSON_GetObjectItem(json, "trainingscounter");
    if (cJSON_IsNumber(trainingsCounterItem)) {
        federatedLearning->trainingscounter = trainingsCounterItem->valueint;
    } else {
        federatedLearning->trainingscounter = 0;  // Valor padrão ou tratamento de erro
    }

    //printf("Training counter %d\n", federatedLearning->trainingscounter);

    cJSON* neuralNetworkItem = cJSON_GetObjectItem(json, "neuralnetwork");
    
    if (cJSON_IsObject(neuralNetworkItem)) {
        federatedLearning->neuralnetwork = (NeuralNetwork*)malloc(sizeof(NeuralNetwork));
        //printf("NeuralNetwork Created\n");

        cJSON* epochItem = cJSON_GetObjectItem(neuralNetworkItem, "epoch");
        if (cJSON_IsNumber(epochItem)) {
            federatedLearning->neuralnetwork->epoch = epochItem->valueint;
            //printf("epoch %d\n", federatedLearning->neuralnetwork->epoch);
        }

        cJSON* alphaItem = cJSON_GetObjectItem(neuralNetworkItem, "alpha");
        if (cJSON_IsNumber(alphaItem)) {
            federatedLearning->neuralnetwork->alpha = alphaItem->valuedouble;
            //printf("alpha %f\n", federatedLearning->neuralnetwork->alpha);
        }

        cJSON* regularizationItem = cJSON_GetObjectItem(neuralNetworkItem, "regularization");
        if (cJSON_IsNumber(regularizationItem)) {
            federatedLearning->neuralnetwork->regularization = regularizationItem->valueint;
            //printf("regularization %d\n", federatedLearning->neuralnetwork->regularization);
        }

        cJSON* lambdaItem = cJSON_GetObjectItem(neuralNetworkItem, "lambda");
        if (cJSON_IsNumber(lambdaItem)) {
            federatedLearning->neuralnetwork->lambda = lambdaItem->valuedouble;
            //printf("lambda %f\n", federatedLearning->neuralnetwork->lambda);
        }

        cJSON* percentualtrainingItem = cJSON_GetObjectItem(neuralNetworkItem, "percentualtraining");
        if (cJSON_IsNumber(percentualtrainingItem)) {
            federatedLearning->neuralnetwork->percentualtraining = percentualtrainingItem->valueint;
            //printf("percentualtraining %d\n", federatedLearning->neuralnetwork->percentualtraining);

        }

        cJSON* lossfunctiontypeItem = cJSON_GetObjectItem(neuralNetworkItem, "lossfunctiontype");
        if (cJSON_IsNumber(lossfunctiontypeItem)) {
            federatedLearning->neuralnetwork->lossfunctiontype = lossfunctiontypeItem->valueint;
            //printf("lossfunctiontype %d\n", federatedLearning->neuralnetwork->lossfunctiontype);

        }

        cJSON* layersItem = cJSON_GetObjectItem(neuralNetworkItem, "layers");
        if (cJSON_IsNumber(layersItem)) {
            federatedLearning->neuralnetwork->layers = layersItem->valueint;
            //printf("layers %d\n", federatedLearning->neuralnetwork->layers);

            cJSON* layersArrayItem = cJSON_GetObjectItem(neuralNetworkItem, "layersArray");
            if (cJSON_IsArray(layersArrayItem)) {
                federatedLearning->neuralnetwork->firstlayer = federatedLearning->neuralnetwork->lastlayer = NULL;
                int i=0;

                cJSON* layerElement;
                cJSON_ArrayForEach(layerElement, layersArrayItem) {
                    //printf("layer: %d", i);
                    i++;
                    Layer* layer = (Layer*)malloc(sizeof(Layer));
                    if (layer == NULL) {
                        return NULL;  
                    }

                    layer->firstneuron = layer->lastneuron = NULL;  
                    layer->nextlayer = layer->previouslayer = NULL;
                    cJSON* neuronsItem = cJSON_GetObjectItem(layerElement, "neurons");

                    if (cJSON_IsNumber(neuronsItem)) {
                        layer->neurons = neuronsItem->valueint;
                    } else {
                        //printf("erro ao assossiar quantidades de neuronios a camada");
                    }
                    //printf(" neurons: %d\n", layer->neurons);

                    if (federatedLearning->neuralnetwork->lastlayer == NULL) {
                        federatedLearning->neuralnetwork->firstlayer = federatedLearning->neuralnetwork->lastlayer = layer;
                    } else {
                        federatedLearning->neuralnetwork->lastlayer->nextlayer = layer;
                        layer->previouslayer = federatedLearning->neuralnetwork->lastlayer;
                        federatedLearning->neuralnetwork->lastlayer = layer;
                    }

                    cJSON* neuronsArrayItem = cJSON_GetObjectItem(layerElement, "neuronsArray");

                    if(cJSON_IsArray(neuronsArrayItem)){
                        int j=0;
                        cJSON* neuronElement;
                        cJSON_ArrayForEach(neuronElement, neuronsArrayItem) {
                            
                            //printf("Neuron: %d", j);
                            j++;

                            Neuron* neuron = (Neuron*)malloc(sizeof(Neuron));
                            if (neuron == NULL) {
                                return NULL;  
                            }

                            neuron->nextneuron = neuron->previousneuron = NULL;
                            neuron->firstweight = neuron->lastweight = NULL;
                            cJSON* neurontypeItem = cJSON_GetObjectItem(neuronElement, "neurontype");

                            if (cJSON_IsString(neurontypeItem)) {
                                strncpy(neuron->neurontype, neurontypeItem->valuestring, sizeof(neuron->neurontype));
                            } else {
                                //printf("erro ao assossiar o tipo do neuronio ao neuronio");

                            }

                                //printf(" type %s",neuron->neurontype);

                            cJSON* weightsItem = cJSON_GetObjectItem(neuronElement, "weights");

                            if (cJSON_IsNumber(weightsItem)) {
                                neuron->weights = weightsItem->valueint;
                            } else {
                                //printf("erro ao assossiar quantidades de pesos ao neuronio");
                            }

                            //printf(" Weights: %d\n", neuron->weights);


                            cJSON* biasItem = cJSON_GetObjectItem(neuronElement, "bias");

                            if (cJSON_IsNumber(biasItem)) {
                                neuron->bias = (float)biasItem->valuedouble;
                            } else {
                                //printf("erro ao assossiar quantidades de bias ao neuronio");
                            }

                            //printf("Bias: %.2f\n", neuron->bias);


                            if (layer->lastneuron == NULL) {
                                layer->firstneuron = layer->lastneuron = neuron;
                            } else {
                                layer->lastneuron->nextneuron = neuron;
                                neuron->previousneuron = layer->lastneuron;
                                layer->lastneuron = neuron;
                            }
                            
                            cJSON* weightsArrayItem = cJSON_GetObjectItem(neuronElement, "weightsArray");
                            if(cJSON_IsArray(weightsArrayItem)){
                                int k=0;
                                cJSON* weightElement;
                                cJSON_ArrayForEach(weightElement, weightsArrayItem) {
                                    //printf("weight: ");
                                    k++;

                                    Weight* weight = (Weight*)malloc(sizeof(Weight));
                                    if (weight == NULL) {
                                        return NULL;  // Falha na alocação de memória
                                    }

                                    weight->weight = (float)weightElement->valuedouble;
                                    weight->previousweight = weight->nextweight = NULL;
                                    //printf(" %.2f\n", weight->weight);


                                    if (neuron->lastweight == NULL) {
                                        neuron->firstweight = neuron->lastweight = weight;
                                    } else {
                                        neuron->lastweight->nextweight = weight;
                                        weight->previousweight = neuron->lastweight;
                                        neuron->lastweight = weight;
                                    }
                                    
                                }     
                            }

                           


                        }
                    }

                }
            } else {
                // Tratar erro conforme necessário
            }
        } else {
            // Tratar erro conforme necessário
        }
    } else {
        // Tratar erro conforme necessário
    }

    //PrintNeuralNetwork(federatedLearning->neuralnetwork);

    return federatedLearning;
}
