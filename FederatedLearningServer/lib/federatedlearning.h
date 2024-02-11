#ifndef FEDERATEDLEARNING_H
#define FEDERATEDLEARNING_H

#include "../lib/cJSON.h"
#include <pthread.h>

typedef struct Weight {
  float weight;
  struct Weight * previousweight, * nextweight;
}Weight;

typedef struct Neuron {
  char neurontype[20];
  int weights;
  float activationfunctionvalue;
  float bias;
  struct Weight * firstweight, * lastweight;
  struct Neuron * nextneuron, * previousneuron;
}Neuron;

typedef struct Layer {
  int neurons;
  struct Neuron * firstneuron, * lastneuron;
  struct Layer * previouslayer, * nextlayer;
}Layer;

typedef struct NeuralNetwork {
  int layers;
  int * neuronsInEachLayer;
  struct Layer * firstlayer, * lastlayer;

}NeuralNetwork;

typedef struct FederatedLearning{
    int globalmodelstatus;
    int trainingscounter;
    NeuralNetwork *neuralnetwork;
}FederatedLearning;


//Methods
FederatedLearning *getFederatedLearningInstance();
void setFederatedLearningGlobalModel();
void PerformanceMetrics(int PercentualEvaluation,int Threshold);
#endif
