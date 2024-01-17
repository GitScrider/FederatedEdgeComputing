#include "../lib/federatedlearning.h"
#include "../lib/JSONConverter.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static pthread_mutex_t singletonMutex = PTHREAD_MUTEX_INITIALIZER;

void PrintNeuralNeuralNetWork(NeuralNetwork * neuralnetwork) {

  int i = 0, j = 0, k = 0;

  Layer * currentlayer = neuralnetwork -> firstlayer;
  while (currentlayer != NULL) {

    i++;
    printf("layer %d neurons: %d\n", i, currentlayer -> neurons);

    Neuron * currentneuron = currentlayer -> firstneuron;

    while (currentneuron != NULL) {

      j++;
      printf("%s neuron %d - weights: %d\n",currentneuron->neurontype , j,currentneuron -> weights);

      Weight * currentweight = currentneuron -> firstweight;

      while (currentweight != NULL) {

        k++;
        printf("%d value: %.10f ", k, currentweight -> weight);

        currentweight = currentweight -> nextweight;
      }
      printf("\n");
      k = 0;
      currentneuron = currentneuron -> nextneuron;
    }
    j = 0;
    currentlayer = currentlayer -> nextlayer;

  }
  i = 0;
  printf("\n");

}

void InitializeNeuralNetWork(NeuralNetwork * neuralnetwork, int * neuronsInEachLayerVector) {

  // Inicialize a semente do gerador de números aleatórios
  srand((unsigned) time(NULL));

  neuralnetwork -> neuronsInEachLayer = (int * ) malloc(neuralnetwork -> layers * sizeof(int));

  for (int i = 0; i < neuralnetwork -> layers; i++) {
    neuralnetwork -> neuronsInEachLayer[i] = neuronsInEachLayerVector[i];
  }

  neuralnetwork -> firstlayer = NULL;
  neuralnetwork -> lastlayer = NULL;

  for (int i = 0; i < neuralnetwork -> layers; i++) {

    Layer * newLayer = (Layer * ) malloc(sizeof(Layer));

    if (newLayer == NULL) {
      fprintf(stderr, "Alocation memory Failure\n");
      exit(1);
    }

    newLayer -> neurons = neuralnetwork -> neuronsInEachLayer[i];
    newLayer -> nextlayer = NULL;
    newLayer -> previouslayer = newLayer -> nextlayer;
    newLayer -> firstneuron = NULL;
    newLayer -> lastneuron = newLayer -> firstneuron;

    if (neuralnetwork -> lastlayer == NULL) {
      neuralnetwork -> firstlayer = newLayer;
    } else {
      neuralnetwork -> lastlayer -> nextlayer = newLayer;
      newLayer -> previouslayer = neuralnetwork -> lastlayer;
    }

    neuralnetwork -> lastlayer = newLayer;

    for (int j = 0; j < newLayer -> neurons; j++) {

      Neuron * newneuron = (Neuron * ) malloc(sizeof(Neuron));

      if (newneuron == NULL) {
        fprintf(stderr, "Alocation memory Failure\n");
        exit(1);
      }

      if (i == 0) {
        strcpy(newneuron->neurontype, "INPUT");
        newneuron -> weights = 0;
      }
      else if(i==neuralnetwork -> layers - 1){
        strcpy(newneuron->neurontype, "OUTPUT");
        newneuron -> weights = neuralnetwork -> neuronsInEachLayer[i-1];

      } 
      else {
        strcpy(newneuron->neurontype, "HIDDEN");
        newneuron -> weights = neuralnetwork -> neuronsInEachLayer[i-1];
      }

      newneuron -> nextneuron = NULL;
      newneuron -> previousneuron = newneuron -> nextneuron;
      newneuron -> firstweight = NULL;
      newneuron -> lastweight = newneuron -> firstweight;
      newneuron -> nextneuron = NULL;
      newneuron -> previousneuron = newneuron -> nextneuron;

      if (neuralnetwork -> lastlayer -> lastneuron == NULL) {

        neuralnetwork -> lastlayer -> firstneuron = newneuron;
      } else {

        neuralnetwork -> lastlayer -> lastneuron -> nextneuron = newneuron;
        newneuron -> previousneuron = neuralnetwork -> lastlayer -> lastneuron;
      }

      neuralnetwork -> lastlayer -> lastneuron = newneuron;

      for (int k = 0; k < newneuron -> weights; k++) {

        Weight * newweight = (Weight * ) malloc(sizeof(Weight));

        if (newweight == NULL) {
          fprintf(stderr, "Alocation memory Failure\n");
          exit(1);
        }

        newweight -> weight = (float)(rand() % 100) / 100.0; // Gere um número aleatório entre 0 e 99 e converta para float entre 0 e 1
        //newweight->weight = 0.5;
        newweight -> nextweight = NULL;
        newweight -> previousweight = newweight -> nextweight;

        if (newneuron -> lastweight == NULL) {
          neuralnetwork -> lastlayer -> lastneuron -> firstweight = newweight;
        } else {
          neuralnetwork -> lastlayer -> lastneuron -> lastweight -> nextweight = newweight;
          newweight -> previousweight = neuralnetwork -> lastlayer -> lastneuron -> lastweight;
        }
        newneuron -> lastweight = newweight;
      }
    }
  }
}


// void FedSGD(FederatedLearning * newinstance ){

// FederatedLearning *federatedLearningInstance = getFederatedLearningInstance();


// }

void setFederatedLearningGlobalModel() {
    FederatedLearning *federatedLearningInstance = getFederatedLearningInstance();
    federatedLearningInstance->neuralnetwork->layers = 4;
    int vector[] = {4, 4, 2, 3};
    InitializeNeuralNetWork(federatedLearningInstance->neuralnetwork, vector);
    federatedLearningInstance->globalmodelstatus=1;
    printf("Neural Network Started!\n");
    //PrintNeuralNeuralNetWork(instance.NeuralNetwork);

    //cJSON* jsonFederatedLearning = federatedLearningToJSON(federatedLearningInstance);

    //Imprimir o JSON resultante
    // char* jsonString = cJSON_Print(jsonFederatedLearning);
    // printf("%s\n", jsonString);

    // Liberar a memória alocada
    // cJSON_Delete(jsonFederatedLearning);
    // free(jsonString);
}

FederatedLearning *getFederatedLearningInstance() {
    static FederatedLearning instance;
    pthread_mutex_lock(&singletonMutex);
    if (instance.neuralnetwork == NULL) {
        // Inicialize a NeuralNetwork conforme necessário
        instance.neuralnetwork = (NeuralNetwork *)malloc(sizeof(NeuralNetwork));
        instance.globalmodelstatus = 0;
        instance.trainingscounter = 0;
        // Outras inicializações
    }
    pthread_mutex_unlock(&singletonMutex);  
    return &instance;
}

