
typedef struct NeuralNetworkSetting {
  int activationfunctiontype;
  int lossfunctiontype;
  NeuralNetwork *NeuralNetwork;
  
}NeuralNetworkSetting;


typedef struct Weight {
  float weight;
  struct Weight * previousweight, * nextweight;
}Weight;

typedef struct Neuron {
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


typedef struct {
    int valor;
} Singleton;

// Função para obter a instância única da estrutura
Singleton* getSingletonInstance() {
    static Singleton instance; // A instância única é estática

    return &instance;
}