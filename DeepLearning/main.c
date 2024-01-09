#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>


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


void printMatriz(float **weights, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            printf("%f ", weights[i][j]);
        }
        printf("\n");
    }
}


// PatternCount: The number of training items or rows in the truth table.
// InputNodes: The number of input neurons.
// OutputNodes: The number of output neurons.
void NeuralNetworkTraining(NeuralNetwork * neuralnetwork, int PatternCount, int InputNodes, int OutputNodes, float LearningRate,int epoch) {

  float trainingsample[InputNodes + OutputNodes];
  float label[OutputNodes];
  float outputdata[OutputNodes];
  float Error = 0.0;

  //aux memory for backpropagation
  float ** weights; // matrix control for weights;
  float * deltafunctions; //delta functions vector
  int  weightscolumn, weightslines, deltafunctionsnumber;


  // variables to cycle through pointers
  Layer * currentlayer = neuralnetwork -> firstlayer;
  Neuron * currentneuron = currentlayer -> firstneuron;
  Weight * currentweight;
  
  
  //for (int TrainingCycle = 0; TrainingCycle < epoch; TrainingCycle++) {

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
      //for(int count =0;count<10;count++){
      //fgets(line, sizeof(line), file);

      // Divide a linha em campos usando a função strtok
      char * token = strtok(line, ",");

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

      // trainingsample[0] = 5.1;
      // trainingsample[1] = 3.5;
      // trainingsample[2] = 1.4;
      // trainingsample[3] = 0.2;
      // trainingsample[4] = 1;
      // trainingsample[5] = 0;
      // trainingsample[6] = 0;

      printf("InputData: [%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f]\n", trainingsample[0], trainingsample[1], trainingsample[2], trainingsample[3], trainingsample[4], trainingsample[5], trainingsample[6]);

      //set the input data on inputdata vector
      currentneuron =  neuralnetwork->firstlayer->firstneuron;
      for (int i = 0; i < InputNodes; i++) {
        currentneuron -> activationfunctionvalue = trainingsample[i];
    
        currentneuron = currentneuron -> nextneuron;
      }

      //set the label on label vector
      for (int i = InputNodes; i < InputNodes + OutputNodes; i++) {
        label[i - InputNodes] = trainingsample[i];

      }


      currentlayer = currentlayer->nextlayer;
      printf("FEEDFOWARD\n");

      //FEEDFOWARD
      for (int layer = 1; layer < neuralnetwork -> layers; layer++) {

          Neuron *previousneuron = currentlayer->previouslayer->firstneuron;
          currentneuron = currentlayer -> firstneuron;
          float Z = 0;

          printf("neurons %d\n",currentlayer->neurons);

          for (int i = 0; i < currentlayer -> neurons; i++) {
            currentweight= currentneuron->firstweight;
            printf("wieghts %d\n",currentneuron->weights);
            for (int j = 0;  j< currentneuron -> weights; j++) {
              Z += currentweight->weight * previousneuron->activationfunctionvalue;
              printf("W %.2f IN %.2f Z %.2f - ",currentweight->weight,previousneuron->activationfunctionvalue,Z);
              currentweight =currentweight->nextweight;
              previousneuron = previousneuron ->nextneuron;

            }
            currentneuron -> activationfunctionvalue = Sigmoid(Z);
            printf("ATIVACAO %.2f\n",currentneuron -> activationfunctionvalue);
            Z = 0;
            currentneuron = currentneuron -> nextneuron;
            previousneuron = currentlayer->previouslayer->firstneuron;
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



      printf("BACKPROPAGATION\n");


      //BACKPROPAGATION

      currentlayer = neuralnetwork -> lastlayer;

      for (int layer = neuralnetwork -> layers ; layer > 1; layer--) {

        //verify the last layer
        if (layer == neuralnetwork -> layers) {
          printf("camada %d \n",layer);
          
          float neuronoutputvalue;
          int labelposition;
          currentneuron = currentlayer -> firstneuron;

          //getting the correct position associated to the label
          for (int i = 0; i < OutputNodes; i++) {
            if (label[i] == 1) {
              labelposition = i;
              break;
            }
          }

          //getting the neuron output where the label position is equal to 1
          for (int i = 0; i < currentlayer -> neurons; i++) {
            if (i == labelposition) {
              neuronoutputvalue = currentneuron -> activationfunctionvalue;
              break;
            }
            currentneuron = currentneuron -> nextneuron;
          }
          printf("%.2f\n",neuronoutputvalue);

          //allocate memory for delta functions
          deltafunctionsnumber = 1;
          deltafunctions = (float * ) malloc(deltafunctionsnumber * sizeof(float));
          deltafunctions[0] = CategoricalCrossEntropyDerivative(label[labelposition], neuronoutputvalue) * SigmoidDerivative(neuronoutputvalue);
          printf("delta %f\n", deltafunctions[0]);

          //alocate memory for weights matrix
          weightslines = currentlayer ->previouslayer -> neurons;
          weightscolumn = 1;

          weights = (float ** ) malloc(weightslines * sizeof(float * ));

          for (int i = 0; i < currentlayer -> previouslayer -> neurons; i++) {
            weights[i] = (float * ) malloc(weightscolumn * sizeof(float));
          }

          currentneuron = currentlayer -> firstneuron;
          Neuron* previousneuron; 
          
          for (int i = 0; i < currentlayer->neurons; i++)
          {
            if(i == labelposition){

              previousneuron = currentlayer->previouslayer->firstneuron;
              currentweight = currentneuron->firstweight;
              for(int j=0;j<currentlayer->previouslayer->neurons ;j++){
                printf("W %f A %f ",currentweight->weight,previousneuron->activationfunctionvalue);
                currentweight->weight +=  - deltafunctions[0] * previousneuron->activationfunctionvalue * LearningRate; 
                printf("D %f Wn %f\n",deltafunctions[0],currentweight->weight);
                weights[j][0] = currentweight -> weight;
                previousneuron =previousneuron->nextneuron;
                currentweight = currentweight->nextweight;
              }
            }

          }

        } 
        else{

          printf("camada %d \n",layer);

          float * deltafunctionsaux = (float * ) malloc(deltafunctionsnumber * sizeof(float));
          memcpy(deltafunctionsaux, deltafunctions, deltafunctionsnumber * sizeof(int));
          deltafunctions = (float * ) realloc(deltafunctions, currentlayer->neurons * sizeof(int));
          float deltafunctionaccumulation;

          printf("DELTAS 1\n");

          currentneuron = currentlayer->firstneuron;
          Neuron * nextneuron = currentlayer->nextlayer->firstneuron;
          for (int i = 0; i < weightslines; i++) {
            deltafunctionaccumulation = 0;
            for (int j = 0; j < weightscolumn; j++) {
              printf("Da%d %f * W %f ",j, deltafunctionsaux[j], weights[i][j]);
              deltafunctionaccumulation += deltafunctionsaux[j] * weights[i][j];
            }
          
            deltafunctions[i] = deltafunctionaccumulation * SigmoidDerivative(currentneuron->activationfunctionvalue);
            printf("A %f Dn%i %f\n",currentneuron->activationfunctionvalue,i, deltafunctions[i]);
            
            currentneuron = currentneuron->nextneuron;
          }



          weightscolumn = weightslines;
          weightslines = currentlayer -> previouslayer -> neurons;
          deltafunctionsnumber = weightslines;
          printf("wl %d wc %d\n",weightslines,weightscolumn);
          weights = (float ** ) malloc(weightslines * sizeof(float * ));
          
          for (int i = 0; i < weightslines; i++) {
            weights[i] = (float * ) malloc(weightscolumn * sizeof(float));
          }

          
          currentneuron = currentlayer->firstneuron;
          Neuron *previousneuron; 
          
          
          for(int i = 0; i< currentlayer->neurons;i++){
            
            currentweight = currentneuron->firstweight;
            previousneuron = currentlayer->previouslayer->firstneuron;
            for (int j = 0; j < currentneuron->weights; j++){
              printf("W %.2f D %.2f A %.2f GW %f",currentweight->weight,deltafunctions[i],previousneuron->activationfunctionvalue,- deltafunctions[i]*previousneuron->activationfunctionvalue*LearningRate);
              currentweight->weight += - deltafunctions[i]*previousneuron->activationfunctionvalue*LearningRate;
              weights[j][i] = currentweight->weight;
              printf(" Wn %f ///",currentweight->weight);
              currentweight = currentweight->nextweight;
              previousneuron = previousneuron->nextneuron;
            }
              printf("\n");
            
            currentneuron = currentneuron -> nextneuron;

          }

          printMatriz(weights,weightslines,weightscolumn);


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

    }

    // Fecha o arquivo
    fclose(file);


    //printf("epoch %d Loss: %f\n",TrainingCycle+1,Error);
    printf("Loss: %f\n",Error);

  //}

}

int main() {

  NeuralNetwork neuralnetwork;
  neuralnetwork.layers = 4;
  int vector[] = {4,4,2,3};

  InitializeNeuralNetWork( & neuralnetwork, vector);
  PrintNeuralNeuralNetWork( & neuralnetwork);
  NeuralNetworkTraining( & neuralnetwork, 2, 4, 3, 0.001,1000);

}
