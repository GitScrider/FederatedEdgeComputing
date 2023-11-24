#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

typedef struct NeuralNetworkSetting {
  int activationfunctiontype;
  int lossfunctiontype;

}
NeuralNetworkSetting;

NeuralNetworkSetting * setting;

typedef struct Weight {
  float weight;
  struct Weight * previousweight, * nextweight;
}
Weight;

typedef struct Neuron {
  int weights;
  float activationfunctionvalue;
  float bias;
  struct Weight * firstweight, * lastweight;
  struct Neuron * nextneuron, * previousneuron;
}
Neuron;

typedef struct Layer {
  int neurons;
  struct Neuron * firstneuron, * lastneuron;
  struct Layer * previouslayer, * nextlayer;
}
Layer;

typedef struct NeuralNetwork {
  int layers;
  int * neuronsInEachLayer;
  struct Layer * firstlayer, * lastlayer;

}
NeuralNetwork;

void PrintNeuralNeuralNetWork(NeuralNetwork * neuralnetwork) {

  int i = 0, j = 0, k = 0;

  Layer * currentlayer = neuralnetwork -> firstlayer;
  while (currentlayer != NULL) {

    i++;
    printf("layer %d neurons: %d\n", i, currentlayer -> neurons);

    Neuron * currentneuron = currentlayer -> firstneuron;

    while (currentneuron != NULL) {

      j++;
      printf("neuron: %d weights: %d\n", j, currentneuron -> weights);

      Weight * currentweight = currentneuron -> firstweight;

      while (currentweight != NULL) {

        k++;
        printf("%d value: %f ", k, currentweight -> weight);

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

      if (i == neuralnetwork -> layers - 1) {
        newneuron -> weights = 0;
      } else {
        newneuron -> weights = neuralnetwork -> neuronsInEachLayer[i + 1];
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

float Sigmoid(float Z) {
  return 1.0 / (1.0 + exp(-Z));
}

float SigmoidDerivative(float a) {
  return a * (1 - a);
}

float CategoricalCrossEntropyDerivative(float y, float a) {
  return -(y / a);
}

float CategoricalCrossEntropy(float * label, float * output, int labelsize) {
  float sum = 0;
  for (int i = 0; i < labelsize; i++) {
    sum += label[i] * log(output[i]);
  }
  return -sum;
}

void NeuralNetworkTraining(NeuralNetwork * neuralnetwork, int PatternCount, int InputNodes, int OutputNodes, float LearningRate,int epoch) {

  // PatternCount: The number of training items or rows in the truth table.
  // InputNodes: The number of input neurons.
  // OutputNodes: The number of output neurons.

  float trainingsample[InputNodes + OutputNodes];
  float label[OutputNodes];
  float inputdata[InputNodes];
  float outputdata[OutputNodes];
      float Error = 0.0;

  for (int TrainingCycle = 0; TrainingCycle < epoch; TrainingCycle++) {

    //Get csv line file data
    //tera um for aqui pra pegar a linha do arquivo;
    FILE * file;
    char line[1024];

    // Abre o arquivo CSV para leitura
    file = fopen("dataset.csv", "r");

    if (file == NULL) {
      perror("Erro ao abrir o arquivo");

    }

    // Lê cada linha do arquivo
    while (fgets(line, sizeof(line), file) != NULL) {
      // Divide a linha em campos usando a função strtok
      char * token = strtok(line, ",");

      // Preenche o vetor inputdata com os valores das próximas 4 colunas
      for (int i = 0; i < 7; i++) {
        token = strtok(NULL, ",");
        if (token != NULL) {
          trainingsample[i] = strtof(token, NULL);
        } else {
          // Lidando com o caso em que não há dados suficientes na linha
          printf("Erro: Não há dados suficientes para preencher InputData na linha.\n");
          break;
        }
      }

      // Aqui você pode processar a linha conforme necessário, usando os dados em record

      // Exemplo: exibindo os dados da linha
      //printf("InputData: [%.1f, %.1f, %.1f, %.1f %.1f, %.1f, %.1f]\n", trainingsample[0], trainingsample[1], trainingsample[2], trainingsample[3], trainingsample[4], trainingsample[5], trainingsample[6]);

    //coloque aqui
      //set the input data on inputdata vector
      for (int i = 0; i < InputNodes; i++) {
        inputdata[i] = trainingsample[i];
      }

      //set the label on label vector
      for (int i = InputNodes; i < InputNodes + OutputNodes; i++) {
        label[i - InputNodes] = trainingsample[i];

      }

      float ** weights; // matrix control for weights;
      float * deltafunctions; //delta functions vector
      float * neuronsoutput; //neurons output vector 

      int labelposition, weightscolumn, weightslines, deltafunctionsnumber;


      // variables to cycle through pointers
      Layer * currentlayer = neuralnetwork -> firstlayer;
      Neuron * currentneuron = currentlayer -> firstneuron;
      Weight * currentweight;

      //FEEDFOWARD
      for (int layer = 0; layer < neuralnetwork -> layers - 1; layer++) {

        //verify the first layer
        if (layer == 0) {

          currentneuron = currentlayer -> firstneuron;
          weightslines = currentlayer -> neurons;
          weightscolumn = currentneuron -> weights;
          //allocate memory for the weight matrix
          weights = (float ** ) malloc(weightslines * sizeof(float * ));
          for (int i = 0; i < weightslines; i++) {
            weights[i] = (float * ) malloc(weightscolumn * sizeof(float));
          }

          //get the weights in the current layer;
          for (int i = 0; i < weightslines; i++) {
            Weight * currentweight = currentneuron -> firstweight;
            for (int j = 0; j < weightscolumn; j++) {
              weights[i][j] = currentweight -> weight;
              currentweight = currentweight -> nextweight;
            }
            currentneuron = currentneuron -> nextneuron;
          }

          float Z = 0;
          currentneuron = currentlayer -> nextlayer -> firstneuron;
          //set the neuron output value 
          for (int i = 0; i < weightscolumn; i++) {
            for (int j = 0; j < weightslines; j++) {
              Z += weights[j][i] * inputdata[j];
            }
            currentneuron -> activationfunctionvalue = Sigmoid(Z);
            Z = 0;
            currentneuron = currentneuron -> nextneuron;
          }

          currentneuron = currentlayer -> firstneuron;

          //set the current entrance data for backpropagation
          for (int layerneuron = 0; layerneuron < currentlayer -> neurons; layerneuron++) {
            currentneuron -> activationfunctionvalue = inputdata[layerneuron];
            currentneuron = currentneuron -> nextneuron;
          }

        } else {

          currentneuron = currentlayer -> firstneuron;
          weightslines = currentlayer -> neurons;
          weightscolumn = currentneuron -> weights;

          //allocate memory for the weight matrix
          weights = (float ** ) malloc(weightslines * sizeof(float * ));
          for (int i = 0; i < weightslines; i++) {
            weights[i] = (float * ) malloc(weightscolumn * sizeof(float));
          }
          //get the weights in the current layer;
          for (int i = 0; i < weightslines; i++) {
            Weight * currentweight = currentneuron -> firstweight;
            for (int j = 0; j < weightscolumn; j++) {
              weights[i][j] = currentweight -> weight;
              currentweight = currentweight -> nextweight;
            }

            currentneuron = currentneuron -> nextneuron;
          }

          float Z = 0;
          Neuron * currentneuronaux;
          currentneuron = currentlayer -> nextlayer -> firstneuron;
          //set the neuron output value 
          for (int i = 0; i < weightscolumn; i++) {
            currentneuronaux = currentlayer -> firstneuron;
            for (int j = 0; j < weightslines; j++) {
              Z += weights[j][i] * currentneuronaux -> activationfunctionvalue;
              currentneuronaux = currentneuronaux -> nextneuron;
            }
            currentneuron -> activationfunctionvalue = Sigmoid(Z);
            Z = 0;
            currentneuron = currentneuron -> nextneuron;
          }
        }
        currentlayer = currentlayer -> nextlayer;
      }
      //End FeedFoward

      currentneuron = neuralnetwork -> lastlayer -> firstneuron;
      //getting the neurons output value for loss function
      for (int i = 0; i < OutputNodes; i++) {
        outputdata[i] = currentneuron -> activationfunctionvalue;
        currentneuron = currentneuron -> nextneuron;
      }
      //calculatting the error
      Error = CategoricalCrossEntropy(label, outputdata, OutputNodes);
      printf("Loss: %f\n", Error);

      currentlayer = neuralnetwork -> lastlayer;
      //getting the correct position associated to the label
      for (int i = 0; i < OutputNodes; i++) {
        if (label[i] == 1) {
          labelposition = i;
          break;
        }
      }

      //BACKPROPAGATION
      for (int layer = 0; layer < neuralnetwork -> layers - 1; layer++) {
        //for(int layer =0; layer<2;layer++){

        //verify the first layer
        if (layer == 0) {
          float neuronoutputvalue;
          currentneuron = currentlayer -> firstneuron;

          //getting the neuron output where the label position is equal to 1
          for (int i = 0; i < currentlayer -> neurons; i++) {
            if (i == labelposition) {
              neuronoutputvalue = currentneuron -> activationfunctionvalue;
              break;
            }
            currentneuron = currentneuron -> nextneuron;
          }

          //allocate memory for delta functions
          deltafunctions = (float * ) malloc(1 * sizeof(float));
          deltafunctionsnumber = 1;
          deltafunctions[0] = CategoricalCrossEntropyDerivative(label[labelposition], neuronoutputvalue) * SigmoidDerivative(neuronoutputvalue);
          //printf("delta %f\n", deltafunctions[0]);

          currentneuron = currentlayer -> previouslayer -> firstneuron;

          weightslines = currentlayer -> previouslayer -> neurons;
          weightscolumn = 1;

          //alocate memory for neurons output
          neuronsoutput = (float * ) malloc(weightslines * sizeof(float));

          //alocate memory for weights matrix
          weights = (float ** ) malloc(weightslines * sizeof(float * ));

          for (int i = 0; i < currentlayer -> previouslayer -> neurons; i++) {
            weights[i] = (float * ) malloc(weightscolumn * sizeof(float));
          }

          int weightcontrol = 0;

          for (int i = 0; i < currentlayer -> previouslayer -> neurons; i++) {

            neuronsoutput[i] = currentneuron -> activationfunctionvalue;
            currentweight = currentneuron -> firstweight;

            //getting the updated weight for the previous layer
            for (int j = 0; j < currentneuron -> weights; j++) {

              if (j == labelposition) {

                currentweight -> weight += -deltafunctions[0] * currentneuron -> activationfunctionvalue * LearningRate;
                //printf("deltafunc %f saida %f taxa aprendizado %f\n", deltafunctions[0], currentneuron -> activationfunctionvalue, LearningRate);
                weights[weightcontrol][0] = currentweight -> weight;
                weightcontrol++;
              }
              //printf("%f ", currentweight -> weight);
              currentweight = currentweight -> nextweight;

            }

          // printf("\n");
            currentneuron = currentneuron -> nextneuron;
          }

        } else {

          //printf("camada 2 \n");

          float * deltafunctionsaux = (float * ) malloc(deltafunctionsnumber * sizeof(float));

          memcpy(deltafunctionsaux, deltafunctions, deltafunctionsnumber * sizeof(int));

          deltafunctions = (float * ) realloc(deltafunctions, weightslines * sizeof(int));

          float deltafunctionaccumulation;

          for (int i = 0; i < weightslines; i++) {
            deltafunctionaccumulation = 0;
            //printf("(");
            for (int j = 0; j < weightscolumn; j++) {
              //("delta: %f * peso: %f ", deltafunctionsaux[j], weights[i][j]);

              deltafunctionaccumulation += deltafunctionsaux[j] * weights[i][j];
            }
            //printf(") * sigmoidd/dx(out) %f ", SigmoidDerivative(neuronsoutput[i]));

            deltafunctions[i] = deltafunctionaccumulation * SigmoidDerivative(neuronsoutput[i]);
            //printf("delta funciton: %f\n", deltafunctions[i]);
          }

          weightscolumn = weightslines;

          weightslines = currentlayer -> previouslayer -> neurons;
          deltafunctionsnumber = weightslines;

          weights = (float ** ) malloc(weightslines * sizeof(float * ));
          for (int i = 0; i < weightslines; i++) {
            weights[i] = (float * ) malloc(weightscolumn * sizeof(float));
          }

          neuronsoutput = (float * ) malloc(weightslines * sizeof(float));

          currentneuron = currentlayer -> previouslayer -> firstneuron;

          for (int i = 0; i < currentlayer -> previouslayer -> neurons; i++) {

            neuronsoutput[i] = currentneuron -> activationfunctionvalue;
            currentweight = currentneuron -> firstweight;

            for (int j = 0; j < currentneuron -> weights; j++) {

              currentweight -> weight += -deltafunctions[j] * neuronsoutput[i] * LearningRate;
              //printf("delta %f saida %f taxaaprendizado %f novo peso %f\n", deltafunctions[j], neuronsoutput[i], LearningRate, currentweight -> weight);
              weights[i][j] = currentweight -> weight;
              //printf("%f ", weights[i][j]);

              currentweight = currentweight -> nextweight;
            }

            //printf("\n");

            currentneuron = currentneuron -> nextneuron;

          }


        if(deltafunctionsaux!=NULL){
          free(deltafunctionsaux);
        }
        
        }

        currentlayer = currentlayer -> previouslayer;

      }

      if(weights!=NULL){
        free(weights);
      }
      if(deltafunctions!=NULL){
        free(deltafunctions);
      }
      if(neuronsoutput!=NULL){
        free(neuronsoutput);
      }

    //fim do coloque aqui


    }

    // Fecha o arquivo
    fclose(file);


    printf("epoch %d Loss: %f\n",TrainingCycle+1,Error);

  }

}

int main() {

  NeuralNetwork neuralnetwork;
  neuralnetwork.layers = 4;
  int vector[] = {4,4,2,3};

  InitializeNeuralNetWork( & neuralnetwork, vector);
  PrintNeuralNeuralNetWork( & neuralnetwork);
  NeuralNetworkTraining( & neuralnetwork, 2, 4, 3, 0.001,30);

}
