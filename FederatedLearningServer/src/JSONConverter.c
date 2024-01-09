#include "../lib/JSONConverter.h"
#include "../lib/federatedlearning.h"

#include <string.h>
#include <stdio.h>

//JSON TO FFEDERATED LEARNING  
cJSON* federatedLearningToJSON(const FederatedLearning* federatedLearning) {
    cJSON* root = cJSON_CreateObject();

    cJSON_AddItemToObject(root, "globalmodelstatus", cJSON_CreateNumber(federatedLearning->globalmodelstatus));
    cJSON_AddItemToObject(root, "trainingscounter", cJSON_CreateNumber(federatedLearning->trainingscounter));

    cJSON* jsonNeuralNetwork = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "NeuralNetwork", jsonNeuralNetwork);

    cJSON_AddItemToObject(jsonNeuralNetwork, "layers", cJSON_CreateNumber(federatedLearning->NeuralNetwork->layers));

    cJSON* layersArray = cJSON_CreateArray();
    struct Layer* currentLayer = federatedLearning->NeuralNetwork->firstlayer;

    while (currentLayer != NULL) {
        cJSON* layerObject = cJSON_CreateObject();
        cJSON_AddItemToObject(layerObject, "neurons", cJSON_CreateNumber(currentLayer->neurons));

        cJSON* neuronsArray = cJSON_CreateArray();
        struct Neuron* currentNeuron = currentLayer->firstneuron;

        while (currentNeuron != NULL) {
            cJSON* neuronObject = cJSON_CreateObject();
            cJSON_AddItemToObject(neuronObject, "neurontype", cJSON_CreateString(currentNeuron->neurontype));
            cJSON_AddItemToObject(neuronObject, "weights", cJSON_CreateNumber(currentNeuron->weights));

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

Weight* JSONToWeight(const cJSON* json) {
    if (json == NULL || !cJSON_IsNumber(json)) {
        return NULL;  // JSON inválido
    }

    Weight* weight = (Weight*)malloc(sizeof(Weight));
    if (weight == NULL) {
        return NULL;  // Falha na alocação de memória
    }

    weight->weight = (float)json->valuedouble;
    weight->previousweight = weight->nextweight = NULL;

    return weight;
}

Neuron* JSONToNeuron(const cJSON* json) {
    if (json == NULL || !cJSON_IsObject(json)) {
        return NULL;  // JSON inválido
    }

    Neuron* neuron = (Neuron*)malloc(sizeof(Neuron));
    if (neuron == NULL) {
        return NULL;  // Falha na alocação de memória
    }

    // Obter os valores das chaves correspondentes
    cJSON* neurontypeItem = cJSON_GetObjectItem(json, "neurontype");
    cJSON* weightsItem = cJSON_GetObjectItem(json, "weights");
    cJSON* weightsArrayItem = cJSON_GetObjectItem(json, "weightsArray");

    if (cJSON_IsString(neurontypeItem)) {
        strncpy(neuron->neurontype, neurontypeItem->valuestring, sizeof(neuron->neurontype));
    } else {
        // Valor inválido ou ausente, trate o erro conforme necessário
    }

    if (cJSON_IsNumber(weightsItem)) {
        neuron->weights = weightsItem->valueint;
    } else {
        // Valor inválido ou ausente, trate o erro conforme necessário
    }

    neuron->firstweight = neuron->lastweight = NULL;  // Inicializar os pesos como vazios

    cJSON* weightElement;
    cJSON_ArrayForEach(weightElement, weightsArrayItem) {
        // Chamada recursiva para converter o JSON de cada peso
        Weight* weight = JSONToWeight(weightElement);
        if (weight != NULL) {
            // Adicionar o peso à lista do Neuron
            if (neuron->lastweight == NULL) {
                neuron->firstweight = neuron->lastweight = weight;
            } else {
                neuron->lastweight->nextweight = weight;
                weight->previousweight = neuron->lastweight;
                neuron->lastweight = weight;
            }
        }
    }

    return neuron;
}

Layer* JSONToLayer(const cJSON* json) {
    if (json == NULL || !cJSON_IsObject(json)) {
        return NULL;  // JSON inválido
    }

    Layer* layer = (Layer*)malloc(sizeof(Layer));
    if (layer == NULL) {
        return NULL;  // Falha na alocação de memória
    }

    // Obter os valores das chaves correspondentes
    cJSON* neuronsItem = cJSON_GetObjectItem(json, "neurons");
    cJSON* neuronsArrayItem = cJSON_GetObjectItem(json, "neuronsArray");

    if (cJSON_IsNumber(neuronsItem)) {
        layer->neurons = neuronsItem->valueint;
    } else {
        // Valor inválido ou ausente, trate o erro conforme necessário
    }

    layer->firstneuron = layer->lastneuron = NULL;  // Inicializar os neurônios como vazios

    cJSON* neuronElement;
    cJSON_ArrayForEach(neuronElement, neuronsArrayItem) {
        // Chamada recursiva para converter o JSON de cada neurônio
        Neuron* neuron = JSONToNeuron(neuronElement);
        if (neuron != NULL) {
            // Adicionar o neurônio à lista da Layer
            if (layer->lastneuron == NULL) {
                layer->firstneuron = layer->lastneuron = neuron;
            } else {
                layer->lastneuron->nextneuron = neuron;
                neuron->previousneuron = layer->lastneuron;
                layer->lastneuron = neuron;
            }
        }
    }

    return layer;
}


NeuralNetwork* JSONToNeuralNetwork(const cJSON* json) {
    if (json == NULL || !cJSON_IsObject(json)) {
        return NULL;  // JSON inválido
    }

    NeuralNetwork* neuralNetwork = (NeuralNetwork*)malloc(sizeof(NeuralNetwork));
    if (neuralNetwork == NULL) {
        return NULL;  // Falha na alocação de memória
    }

    // Obter o valor da chave "layers"
    cJSON* layersItem = cJSON_GetObjectItem(json, "layers");
    if (cJSON_IsNumber(layersItem)) {
        neuralNetwork->layers = layersItem->valueint;
    } else {
        neuralNetwork->layers = 0;  // Valor padrão ou tratamento de erro
    }

    // Obter o valor da chave "layersArray"
    cJSON* layersArrayItem = cJSON_GetObjectItem(json, "layersArray");
    if (cJSON_IsArray(layersArrayItem)) {
        neuralNetwork->firstlayer = neuralNetwork->lastlayer = NULL;  // Inicializar as camadas como vazias

        cJSON* layerElement;
        cJSON_ArrayForEach(layerElement, layersArrayItem) {
            // Chamada recursiva para converter o JSON de cada camada
            Layer* layer = JSONToLayer(layerElement);
            if (layer != NULL) {
                // Adicionar a camada à lista da NeuralNetwork
                if (neuralNetwork->lastlayer == NULL) {
                    neuralNetwork->firstlayer = neuralNetwork->lastlayer = layer;
                } else {
                    neuralNetwork->lastlayer->nextlayer = layer;
                    layer->previouslayer = neuralNetwork->lastlayer;
                    neuralNetwork->lastlayer = layer;
                }
            }
        }
    } else {
        // Valor inválido ou ausente, trate o erro conforme necessário
    }

    return neuralNetwork;
}


FederatedLearning* JSONToFederatedLearning(const cJSON* json) {
    if (json == NULL || !cJSON_IsObject(json)) {
      printf("nao foi possivel converter");
        return NULL;  // JSON inválido
    }

    FederatedLearning* federatedLearning = (FederatedLearning*)malloc(sizeof(FederatedLearning));
    if (federatedLearning == NULL) {
        return NULL;  // Falha na alocação de memória
    }

    // Obter o valor da chave "globalmodelstatus"
    cJSON* statusItem = cJSON_GetObjectItem(json, "globalmodelstatus");
    if (cJSON_IsNumber(statusItem)) {
        federatedLearning->globalmodelstatus = statusItem->valueint;
    } else {
        federatedLearning->globalmodelstatus = 0;  // Valor padrão ou tratamento de erro
    }

    // Obter o valor da chave "trainingscounter"
    cJSON* trainingsCounterItem = cJSON_GetObjectItem(json, "trainingscounter");
    if (cJSON_IsNumber(trainingsCounterItem)) {
        federatedLearning->trainingscounter = trainingsCounterItem->valueint;
        printf("\n\n%d\n\n",federatedLearning->trainingscounter);
    } else {
        federatedLearning->trainingscounter = 0;  // Valor padrão ou tratamento de erro
    }

    // Obter o valor da chave "neuronnetwork"
    cJSON* neuralNetworkItem = cJSON_GetObjectItem(json, "neuronnetwork");
    if (cJSON_IsObject(neuralNetworkItem)) {
        // Chamada recursiva para converter o JSON da NeuralNetwork
        federatedLearning->NeuralNetwork = JSONToNeuralNetwork(neuralNetworkItem);
    } else {
        // Valor inválido ou ausente, inicialize com um NeuralNetwork vazio ou trate o erro conforme necessário
        federatedLearning->NeuralNetwork = NULL;
    }

    PrintNeuralNeuralNetWork(federatedLearning->NeuralNetwork);

    return federatedLearning;
}