#ifndef _federatedlearning
#define _federatedlearning


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



FederatedLearning *getFederatedLearningInstance();
void replaceNeuralNetwork(FederatedLearning * newfederatedlearninginstance);
void mergeNeuralNetwork(FederatedLearning * newfederatedlearninginstance);
void PrintNeuralNetwork(NeuralNetwork *neuralnetwork);

void NeuralNetworkTraining(NeuralNetwork * neuralnetwork, int InputNodes, int OutputNodes, float LearningRate,int epoch);

void teste();

#endif