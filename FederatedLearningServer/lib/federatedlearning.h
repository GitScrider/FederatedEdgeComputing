#ifndef FEDERATEDLEARNING_H
#define FEDERATEDLEARNING_H

#include "../lib/cJSON.h"
#include <pthread.h>

//activation function
#define PERCEPTRON 1
#define RELU 2
#define SIGMOID 3

//output activation function
#define SOFTMAX 4

//loss function
#define MINIMAL_MEAN_SQUARE 1
#define CATEGORICAL_CROSS_ENTROPY 2

//regularization
#define NONE_REGULARIZATION 0
#define L1 1
#define L2 2

//weight value
#define WEIGHT_VALUE_ZERO 0
#define WEIGHT_VALUE_RANDOM 1
#define WEIGHT_VALUE_HALF 2

//////////

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
  int activationfunctiontype;
  int neurons;
  struct Neuron * firstneuron, * lastneuron;
  struct Layer * previouslayer, * nextlayer;
}Layer;

typedef struct NeuralNetwork {
  int epoch;
  float alpha;
  int regularization;
  float lambda;
  int percentualtraining;
  int lossfunctiontype;
  int layers;
  struct Layer * firstlayer, * lastlayer;
}NeuralNetwork;

//////////

typedef struct ClientNode{
  char ip_id[15];
  int interaction;
  struct ClientNode *previousclientnode, *nextclientnode;
}ClientNode;

typedef struct NodeControl{
  int currentinteraction;
  int interactioncycle;
  int clientnodes;
  int clientnodesregistered;
  NeuralNetwork *neuralnetwork;
  struct ClientNode *firstclientnode, *lastclientnode;
}NodeControl;

//////////

typedef struct FederatedLearning{
    int globalmodelstatus;
    int trainingscounter;
    NeuralNetwork *neuralnetwork;
    NodeControl *nodecontrol;
}FederatedLearning;

//////////

typedef struct LayerConfig{
  struct LayerConfig *first, *next;
  int neurons;
  int activationfunctiontype;  
}LayerConfig;

//Methods
FederatedLearning *getFederatedLearningInstance();
void PrintNeuralNeuralNetwork(NeuralNetwork * neuralnetwork);
void setFederatedLearningGlobalModel();
void AggregationModel(FederatedLearning * clientmodel);
void PerformanceMetrics(NeuralNetwork * neuralnetwork,int PercentualEvaluation,float Threshold);
#endif
