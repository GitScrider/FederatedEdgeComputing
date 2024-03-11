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
      printf("bias %f\n",currentneuron->bias);

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
        //newneuron -> bias = (float)(rand() % 100) / 100.0; 
        newneuron -> bias = 0.0;
      } 
      else {
        strcpy(newneuron->neurontype, "HIDDEN");
        newneuron -> weights = neuralnetwork -> neuronsInEachLayer[i-1];
        //newneuron -> bias = (float)(rand() % 100) / 100.0; 
        newneuron -> bias = 0.0;
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

float SoftMax(float Zsum , float Z){
  return Z/Zsum;
}

float SoftMaxDerivative1(float a){
  
  return a*(1-a);
}

float SoftMaxDerivative2(float a1 , float a2){
  
  return a1*a2;

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

void freeWeight(Weight *weight) {
    if(weight!=NULL){
        free(weight);        
    }
}

void freeNeuron(Neuron *neuron) {
    if(neuron!=NULL){
        free(neuron);        
    }
}

void freeLayer(Layer *layer) {
    if(layer!=NULL){
        free(layer);        
    }
}

void freeNeuralNetwork(NeuralNetwork neuralnetwork) {
      //printf("teste 0");
    
    Layer *currentlayer = neuralnetwork.firstlayer;

    while (currentlayer != NULL) {
      //printf("teste 1");
        // Libere a memória dos neurônios na camada atual
        Neuron *currentneuron = currentlayer->firstneuron;
        while (currentneuron != NULL) {
            //printf("teste 2");

            // Libere a memória dos pesos associados ao neurônio atual
            Weight *currentweight = currentneuron->firstweight;
            while (currentweight != NULL) {
                //printf("teste 3");

                Weight *nextweight = currentweight->nextweight;
                freeWeight(currentweight);
                currentweight = nextweight;
            }

            // Libere a memória do neurônio atual
            Neuron *nextneuron = currentneuron->nextneuron;
            freeNeuron(currentneuron);
            currentneuron = nextneuron;
        }

        // Libere a memória da camada atual
        Layer *nextlayer = currentlayer->nextlayer;
        freeLayer(currentlayer);
        currentlayer = nextlayer;
    }

    // Libere a memória da NeuralNetwork
    free(neuralnetwork.neuronsInEachLayer);
    //free(neuralnetwork);
}

void freeVector(float *vector) {
    if (vector != NULL) {
        free(vector);
    }
}

void freeMatrix(float **matrix, int lines) {
    if (matrix != NULL) {
        for (int i = 0; i < lines; i++) {
            if (matrix[i] != NULL) {
                free(matrix[i]);
            }
        }
        free(matrix);
    }
}

void printVector(float *data,int size){
  for(int i =0; i< size;i++){
    printf("%f ", data[i]);
  }
  printf("\n");
}

void printMatriz(float **data, int linhas, int colunas) {
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            printf("%f ", data[i][j]);
        }
        printf("\n");
    }
}

// InputNodes: The number of input neurons.
// OutputNodes: The number of output neurons.
void NeuralNetworkTraining(NeuralNetwork * neuralnetwork, float LearningRate,int Epoch, int PercentualTraining) {
  
  float trainingsample[neuralnetwork->firstlayer->neurons + neuralnetwork->lastlayer->neurons];
  float label[neuralnetwork->lastlayer->neurons];
  float outputdata[neuralnetwork->lastlayer->neurons];
  float Error = 0.0;

  FILE * file = NULL;
  char line[1024];

  //aux memory for backpropagation
  int  weightscolumn=0, weightslines=0, deltafunctionsnumber=0;
  //float ** weights = malloc(weightslines * sizeof(float *)); // matrix control for weights;
  //float * deltafunctions = malloc(deltafunctionsnumber * sizeof(float)); //vector control for delta functions
  float **weights = NULL;
  float *deltafunctions = NULL;
  // variables to cycle through pointers
  Layer * currentlayer = neuralnetwork -> firstlayer;
  Neuron * currentneuron = currentlayer -> firstneuron;
  Weight * currentweight;
  
  for (int TrainingCycle = 0; TrainingCycle < Epoch; TrainingCycle++) {

    //Abre o arquivo CSV para leitura
    file = fopen("dataset/datasettraining.csv", "r");

    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }


    for(int count =0; count < PercentualTraining; count++){
      fgets(line, sizeof(line), file);

      // Divide a linha em campos usando a função strtok
        char * token = strtok(line, ","); 

        for (int i = 0; i < 7; i++){
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

      //  trainingsample[0] = 5.1;
      //  trainingsample[1] = 3.5;
      //  trainingsample[2] = 1.4;
      //  trainingsample[3] = 0.2;
      //  trainingsample[4] = 1;
      //  trainingsample[5] = 0;
      //  trainingsample[6] = 0;

      //printf("InputData: [%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f]\n", trainingsample[0], trainingsample[1], trainingsample[2], trainingsample[3], trainingsample[4], trainingsample[5], trainingsample[6]);

      //set the input data on inputdata vector
      currentneuron =  neuralnetwork->firstlayer->firstneuron;
      for (int i = 0; i < neuralnetwork->firstlayer->neurons; i++) {
        currentneuron -> activationfunctionvalue = trainingsample[i];
    
        currentneuron = currentneuron -> nextneuron;
      }

      //set the label on label vector

      for (int i = neuralnetwork->firstlayer->neurons; i < neuralnetwork->firstlayer->neurons + neuralnetwork->lastlayer->neurons; i++) {
        label[i - neuralnetwork->firstlayer->neurons] = trainingsample[i];

      }

      //printf("FEEDFOWARD\n");
      //FEEDFOWARD
      currentlayer = currentlayer->nextlayer;
      for (int layer = 1; layer < neuralnetwork -> layers; layer++) {
            
            Neuron *previousneuron = currentlayer->previouslayer->firstneuron;
            currentneuron = currentlayer -> firstneuron;
            float Z = 0;
          
          if (layer == neuralnetwork->layers -1){
            //printf("neurons %d\n",currentlayer->neurons);


            float softmaxsum=0;

            for (int i = 0; i < currentlayer -> neurons; i++) {
              currentweight= currentneuron->firstweight;
             //printf("wieghts %d \nB %f - ",currentneuron->weights,currentneuron->bias);
              Z+= currentneuron->bias;
              for (int j = 0;  j< currentneuron -> weights; j++) {
                Z += currentweight->weight * previousneuron->activationfunctionvalue;
                //printf("W %f IN %f - ",currentweight->weight,previousneuron->activationfunctionvalue,Z);
                currentweight =currentweight->nextweight;
                previousneuron = previousneuron ->nextneuron;

              }
              softmaxsum += exp(Z);
             // printf("Exp Z %f SOFTMAXSUM += %f\n",exp(Z),softmaxsum);

              Z = 0;
              currentneuron = currentneuron -> nextneuron;
              previousneuron = currentlayer->previouslayer->firstneuron;
            }

            previousneuron = currentlayer->previouslayer->firstneuron;
            currentneuron = currentlayer -> firstneuron;
            
            for (int i = 0; i < currentlayer -> neurons; i++) {
              currentweight= currentneuron->firstweight;
              //printf("wieghts %d \nB %f - ",currentneuron->weights,currentneuron->bias);
              Z+= currentneuron->bias;
              for (int j = 0;  j< currentneuron -> weights; j++) {
                Z += currentweight->weight * previousneuron->activationfunctionvalue;
                //printf("W %f IN %f - ",currentweight->weight,previousneuron->activationfunctionvalue);
                currentweight =currentweight->nextweight;
                previousneuron = previousneuron ->nextneuron;

              }
              currentneuron -> activationfunctionvalue = SoftMax(softmaxsum,exp(Z));
              //printf("Exp Z %f ATIVACAO %f\n",exp(Z), currentneuron -> activationfunctionvalue);
              Z = 0;
              currentneuron = currentneuron -> nextneuron;
              previousneuron = currentlayer->previouslayer->firstneuron;
            }


          }
          else{

            for (int i = 0; i < currentlayer -> neurons; i++) {
              currentweight= currentneuron->firstweight;
              //printf("wieghts %d \nB %f - ",currentneuron->weights,currentneuron->bias);
              Z+= currentneuron->bias;
              for (int j = 0;  j< currentneuron -> weights; j++) {
                Z += currentweight->weight * previousneuron->activationfunctionvalue;
                //printf("W %f IN %f + ",currentweight->weight,previousneuron->activationfunctionvalue);
                currentweight =currentweight->nextweight;
                previousneuron = previousneuron ->nextneuron;

              }
              currentneuron -> activationfunctionvalue = Sigmoid(Z);

              //printf("Z %f ATIVACAO %f\n",Z,currentneuron -> activationfunctionvalue);
              Z = 0;
              currentneuron = currentneuron -> nextneuron;
              previousneuron = currentlayer->previouslayer->firstneuron;
            }
          }
          
        currentlayer = currentlayer -> nextlayer;
      }
      //End FeedFoward

      //LOSS CALCULATION
      currentneuron = neuralnetwork -> lastlayer -> firstneuron;
      //getting the neurons output value for loss function
      for (int i = 0; i < neuralnetwork->lastlayer->neurons; i++) {
        outputdata[i] = currentneuron -> activationfunctionvalue;
        currentneuron = currentneuron -> nextneuron;
        //printf("label %f output %f \n",label[i],outputdata[i]);
      }
      Error = CategoricalCrossEntropy(label, outputdata, neuralnetwork->lastlayer->neurons);
      //printf("Loss: %f\n", Error);

      //END LOSSCALCULATION

      //printf("BACKPROPAGATION\n");

      //BACKPROPAGATION
      currentlayer = neuralnetwork -> lastlayer;
      for (int layer = neuralnetwork -> layers ; layer > 1; layer--) {

        //verify the last layer
        if (layer == neuralnetwork -> layers) {

          //printf("layer %d \n",layer);

          //allocate memory for delta functions
          deltafunctionsnumber = currentlayer->neurons;
          deltafunctions = (float * ) malloc(deltafunctionsnumber * sizeof(float));
          currentneuron = currentlayer -> firstneuron;


          //getting the correct delta function associated to the neuron output

          // for (int i = 0; i < currentlayer->neurons; i++) {
          //   if (label[i] == 1) {
          //     deltafunctions[i] =  -(1-currentneuron->activationfunctionvalue);
          //   }
          //   else{
          //     deltafunctions[i] = - currentneuron->activationfunctionvalue;
          //   }
          //   printf("output value %f delta %f\n",currentneuron -> activationfunctionvalue,deltafunctions[i]);

          //   currentneuron = currentneuron -> nextneuron;
          // }


          for (int i = 0; i < currentlayer->neurons; i++) {

              deltafunctions[i] =  currentneuron->activationfunctionvalue - label[i];

            //printf("output value %f delta %f\n",currentneuron -> activationfunctionvalue,deltafunctions[i]);

            currentneuron = currentneuron -> nextneuron;
          }



          //alocate memory for weights matrix
          weightslines = currentlayer ->previouslayer -> neurons;
          weightscolumn = currentlayer->neurons;

          weights = (float ** ) malloc(weightslines * sizeof(float * ));

          for (int i = 0; i < currentlayer -> previouslayer -> neurons; i++) {
            weights[i] = (float * ) malloc(weightscolumn * sizeof(float));
          }

          currentneuron = currentlayer -> firstneuron;
          Neuron* previousneuron; 
          
          for (int i = 0; i < currentlayer->neurons; i++){
          
              currentneuron->bias += - deltafunctions[i] * LearningRate;

              previousneuron = currentlayer->previouslayer->firstneuron;
              currentweight = currentneuron->firstweight;

              for(int j=0;j<currentlayer->previouslayer->neurons ;j++){
                //printf("W %f A %f ",currentweight->weight,previousneuron->activationfunctionvalue);
                currentweight->weight +=  - deltafunctions[i] * previousneuron->activationfunctionvalue * LearningRate; 
                //printf("D %f Wn %f\n",deltafunctions[i],currentweight->weight);
                weights[j][i] = currentweight -> weight;
                previousneuron =previousneuron->nextneuron;
                currentweight = currentweight->nextweight;
              }
            

            currentneuron = currentneuron->nextneuron;

          }

        } 
        else{

          //printf("layer %d \n",layer);
          float * deltafunctionsaux = (float * ) malloc(deltafunctionsnumber * sizeof(float));
          memcpy(deltafunctionsaux, deltafunctions, deltafunctionsnumber * sizeof(float));
          deltafunctions = (float * ) realloc(deltafunctions, currentlayer->neurons * sizeof(int));
          
          float deltafunctionaccumulation;

          //calculatting delta
          currentneuron = currentlayer->firstneuron;

          for (int i = 0; i < weightslines; i++) {
            deltafunctionaccumulation = 0;
            for (int j = 0; j < weightscolumn; j++) {
              //printf("Dn %d D %f * W %f \n",j, deltafunctionsaux[j], weights[i][j]);
              deltafunctionaccumulation += deltafunctionsaux[j] * weights[i][j];
            }
          
            deltafunctions[i] = deltafunctionaccumulation * SigmoidDerivative(currentneuron->activationfunctionvalue);
            //printf("A %f Dn%i %f\n",currentneuron->activationfunctionvalue,i, deltafunctions[i]);
            
            currentneuron = currentneuron->nextneuron;
          }

          freeMatrix(weights,weightslines);

          weightscolumn = weightslines;
          weightslines = currentlayer -> previouslayer -> neurons;
          deltafunctionsnumber = weightslines;
          
          //printf("wl %d wc %d\n",weightslines,weightscolumn);
          
          weights = (float ** ) malloc(weightslines * sizeof(float * ));          
          for (int i = 0; i < weightslines; i++) {
            weights[i] = (float * ) malloc(weightscolumn * sizeof(float));
          }

          currentneuron = currentlayer->firstneuron;
          Neuron *previousneuron;
          
          for(int i = 0; i< currentlayer->neurons;i++){
            
            currentneuron->bias += - deltafunctions[i]*LearningRate;

            currentweight = currentneuron->firstweight;
            previousneuron = currentlayer->previouslayer->firstneuron;
            for (int j = 0; j < currentneuron->weights; j++){
              //printf("W %f D %f A %f GW %f",currentweight->weight,deltafunctions[i],previousneuron->activationfunctionvalue,- deltafunctions[i]*previousneuron->activationfunctionvalue*LearningRate);
              currentweight->weight += - deltafunctions[i]*previousneuron->activationfunctionvalue*LearningRate;
              weights[j][i] = currentweight->weight;
              //printf(" Wn %f \n",currentweight->weight);
              currentweight = currentweight->nextweight;
              previousneuron = previousneuron->nextneuron;
            }
              //printf("\n");
            
            currentneuron = currentneuron -> nextneuron;

          }

          //printMatriz(weights,weightslines,weightscolumn);


        freeVector(deltafunctionsaux);
        
        }

        currentlayer = currentlayer -> previouslayer;

      }

      freeMatrix(weights,weightslines);
      freeVector(deltafunctions);

    //printf("Loss: %f\n",Error);
    //size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_8BIT);
    //printf("Free Heap Size: %u bytes\n", free_heap);


    }

    // Fecha o arquivo
    fclose(file);

    printf("Epoch %d Loss: %f\n",TrainingCycle+1,Error);
    //printf("Epoch %d \n",TrainingCycle+1);

  }



}

void PerformanceMetrics(NeuralNetwork * neuralnetwork,int PercentualEvaluation,int Threshold){
  
  float trainingsample[neuralnetwork->firstlayer->neurons + neuralnetwork->lastlayer->neurons];
  float label[neuralnetwork->lastlayer->neurons];
  float outputdata[neuralnetwork->lastlayer->neurons];
  float Precision=0, Recall=0, Accuracy=0,Specificity=0,F1Score=0;


  int total=0,truepositive=0,falsepositive=0,truenegative=0,falsenegative=0;

  // variables to cycle through pointers
  Layer * currentlayer = neuralnetwork -> firstlayer;
  Neuron * currentneuron = currentlayer -> firstneuron;
  Weight * currentweight;

  FILE * file = NULL;
  char line[1024];

  //Abre o arquivo CSV para leitura
    file = fopen("dataset/datasetevaluation.csv", "r");

    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }

  for(int count =0;count<PercentualEvaluation;count++){
      fgets(line, sizeof(line), file);

        // Divide a linha em campos usando a função strtok
        char * token = strtok(line, ","); 

        for (int i = 0; i < 7; i++){
          token = strtok(NULL, ",");
          if (token != NULL) {
            trainingsample[i] = strtof(token, NULL);
          } else {
            // Lidando com o caso em que não há dados suficientes na linha
            printf("Erro: Não há dados suficientes para preencher InputData na linha.\n");
            break;
          }

        }

        printf("InputData: [%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f]\n", trainingsample[0], trainingsample[1], trainingsample[2], trainingsample[3], trainingsample[4], trainingsample[5], trainingsample[6]);
        
        currentneuron =  neuralnetwork->firstlayer->firstneuron;
        for (int i = 0; i < neuralnetwork->firstlayer->neurons; i++) {
          currentneuron -> activationfunctionvalue = trainingsample[i];
          currentneuron = currentneuron -> nextneuron;
        }

        //set the label on label vector
        for (int i = neuralnetwork->firstlayer->neurons; i < neuralnetwork->firstlayer->neurons + neuralnetwork->lastlayer->neurons; i++) {
          label[i - neuralnetwork->firstlayer->neurons] = trainingsample[i];
          
        }

      printf("FEEDFOWARD\n");
      //FEEDFOWARD
      currentlayer = neuralnetwork->firstlayer->nextlayer;
      for (int layer = 1; layer < neuralnetwork -> layers; layer++) {

          Neuron *previousneuron = currentlayer->previouslayer->firstneuron;
          currentneuron = currentlayer -> firstneuron;
          float Z = 0;

          //printf("neurons %d\n",currentlayer->neurons);

          for (int i = 0; i < currentlayer -> neurons; i++) {
            currentweight= currentneuron->firstweight;
            //printf("wieghts %d bias %f\n",currentneuron->weights,currentneuron->bias);
            Z+= currentneuron->bias;
            for (int j = 0;  j< currentneuron -> weights; j++) {
              Z += currentweight->weight * previousneuron->activationfunctionvalue;
              //printf("W %f IN %f Z %f - ",currentweight->weight,previousneuron->activationfunctionvalue,Z);
              currentweight =currentweight->nextweight;
              previousneuron = previousneuron ->nextneuron;

            }
            currentneuron -> activationfunctionvalue = Sigmoid(Z);
            //printf("ATIVACAO %f\n",currentneuron -> activationfunctionvalue);
            Z = 0;
            currentneuron = currentneuron -> nextneuron;
            previousneuron = currentlayer->previouslayer->firstneuron;
          }

        currentlayer = currentlayer -> nextlayer;
      }
        
      

      currentneuron = neuralnetwork->lastlayer->firstneuron;

      for(int i=0;i< neuralnetwork->lastlayer->neurons;i++){
        // printf("%f ",currentneuron->activationfunctionvalue);
        // printf("%f\n",label[i]);

        if(label[i]==1){
          // printf(" positivo\n");
          if(currentneuron->activationfunctionvalue > Threshold){
            truepositive++;
          }else{
            falsepositive++;
          }
          total++;
          
        }else{
          printf("negativo\n");

          if(currentneuron->activationfunctionvalue < Threshold){
            truenegative++;
          }else{
            falsenegative++;
          }
          total++;

        }
        currentneuron = currentneuron->nextneuron;

      }
      printf("\n");
      

  }

  fclose(file);

      printf("total = %d, TP = %d, TN = %d, FP = %d, FN = %d\n",total,truepositive,truenegative,falsepositive,falsenegative);

      Accuracy = (float)(truenegative+truepositive)/(truenegative+truepositive+falsenegative+falsepositive);
      Precision = (float)truepositive/(truepositive+falsepositive);
      Recall =(float)truepositive/(truepositive+falsenegative);
      Specificity = (float)truenegative/(truenegative+falsepositive);
      F1Score = 2*(float)(Precision * Recall)/(Precision + Recall);
      printf("Accuracy: %.2f\n",30.0/90.0);
      printf("Precision: %.2f\n",Precision);
      printf("Recall: %.2f\n",Recall);
      printf("Specificity: %.2f\n",Specificity);
      printf("F1-Score: %.2f\n",F1Score);
}

int main() {

  NeuralNetwork neuralnetwork;
  neuralnetwork.layers = 4;
  int vector[] = {4,8,16,3};

  InitializeNeuralNetWork( & neuralnetwork, vector);
  PrintNeuralNeuralNetWork( & neuralnetwork);
  NeuralNetworkTraining( & neuralnetwork, 0.1,100,120);
  //PrintNeuralNeuralNetWork( & neuralnetwork);

  //PerformanceMetrics(&neuralnetwork,30,0.5);

  freeNeuralNetwork( neuralnetwork);

}


