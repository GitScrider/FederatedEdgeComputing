#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "federatedlearning.h"


#include "esp_log.h"
#include "esp_heap_caps.h"


void teste( ){

    printf("🐉\n");
}

void PrintNeuralNetwork(NeuralNetwork * neuralnetwork) {

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


void replaceNeuralNetwork(FederatedLearning * newfederatedlearninginstance){
    FederatedLearning * federatedlearninginstance =  getFederatedLearningInstance();
    federatedlearninginstance->neuralnetwork = newfederatedlearninginstance->neuralnetwork;
    federatedlearninginstance->trainingscounter=0;
}

void mergeNeuralNetwork(FederatedLearning * newfederatedlearninginstance){

}


FederatedLearning *getFederatedLearningInstance() {
    
    static FederatedLearning instance;
    if (instance.neuralnetwork == NULL) {
        // Inicialize a NeuralNetwork conforme necessário
        instance.neuralnetwork = (NeuralNetwork *)malloc(sizeof(NeuralNetwork));
        instance.globalmodelstatus = 0;
        instance.trainingscounter = 0;
        // Outras inicializações
    }

    return &instance;
}

//training algorithm


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
    file = fopen("/storage/dataset.csv", "r");

    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    // Lê cada linha do arquivo
    //while (fgets(line, sizeof(line), file) != NULL) {
    for(int count =0;count<PercentualTraining;count++){
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
          //freeVector(deltafunctions);
          //allocate memory for delta functions
          deltafunctionsnumber = 1;
          deltafunctions = (float * ) malloc(deltafunctionsnumber * sizeof(float));
          currentneuron = currentlayer -> firstneuron;

          //getting the correct position associated to the label
          for (int i = 0; i < currentlayer->neurons; i++) {
            if (label[i] == 1) {
              deltafunctions[0] = CategoricalCrossEntropyDerivative(label[i], currentneuron -> activationfunctionvalue) * SigmoidDerivative(currentneuron -> activationfunctionvalue);
              break;
            }
            currentneuron = currentneuron -> nextneuron;
          }
          //printf("output value %f delta %f\n",currentneuron -> activationfunctionvalue,deltafunctions[0]);


          //alocate memory for weights matrix
          
          // if(weights!=NULL){
          //   free(weights);
          // }

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
            if(label[i] == 1){

              currentneuron->bias += - deltafunctions[0] * LearningRate;

              previousneuron = currentlayer->previouslayer->firstneuron;
              currentweight = currentneuron->firstweight;
              for(int j=0;j<currentlayer->previouslayer->neurons ;j++){
                //printf("W %f A %f ",currentweight->weight,previousneuron->activationfunctionvalue);
                currentweight->weight +=  - deltafunctions[0] * previousneuron->activationfunctionvalue * LearningRate; 
                //printf("D %f Wn %f\n",deltafunctions[0],currentweight->weight);
                weights[j][0] = currentweight -> weight;
                previousneuron =previousneuron->nextneuron;
                currentweight = currentweight->nextweight;
              }
            }

            currentneuron = currentneuron->nextneuron;

          }

        } 
        else{
          //printf("camada %d \n",layer);

          float * deltafunctionsaux = (float * ) malloc(deltafunctionsnumber * sizeof(float));
          memcpy(deltafunctionsaux, deltafunctions, deltafunctionsnumber * sizeof(float));
          deltafunctions = (float * ) realloc(deltafunctions, currentlayer->neurons * sizeof(int));
          
          float deltafunctionaccumulation;

          //printf("DELTAS 1\n");

          //calculatting delta
          currentneuron = currentlayer->firstneuron;

          for (int i = 0; i < weightslines; i++) {
            deltafunctionaccumulation = 0;
            for (int j = 0; j < weightscolumn; j++) {
              //printf("Da%d %f * W %f ",j, deltafunctionsaux[j], weights[i][j]);
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


// InputNodes: The number of input neurons.
// OutputNodes: The number of output neurons.
// void NeuralNetworkTraining(NeuralNetwork * neuralnetwork, int InputNodes, int OutputNodes, float LearningRate,int epoch) {

//   float trainingsample[InputNodes + OutputNodes];
//   float label[OutputNodes];
//   float outputdata[OutputNodes];
//   //float Error = 0.0;

//   //aux memory for backpropagation
//   int  weightscolumn=0, weightslines=0, deltafunctionsnumber=0;
//   float ** weights = malloc(weightslines * sizeof(float *)); // matrix control for weights;
//   float * deltafunctions = malloc(deltafunctionsnumber * sizeof(float)); //vector control for delta functions
//   // variables to cycle through pointers
//   Layer * currentlayer = neuralnetwork -> firstlayer;
//   Neuron * currentneuron = currentlayer -> firstneuron;
//   Weight * currentweight;
  


//   //for (int TrainingCycle = 0; TrainingCycle < epoch; TrainingCycle++) {

//     FILE * file;
//     char line[1024];

//     //Abre o arquivo CSV para leitura
//     file = fopen("/storage/dataset.csv", "r");

//     if (file == NULL) {
//         perror("Erro ao abrir o arquivo");
//         return;
//     }

//     // Lê cada linha do arquivo
//     while (fgets(line, sizeof(line), file) != NULL) {
//         // for(int count =0;count<10;count++){
//         // fgets(line, sizeof(line), file);

//       // Divide a linha em campos usando a função strtok
//         char * token = strtok(line, ",");

//         for (int i = 0; i < 7; i++){
//             token = strtok(NULL, ",");
//             if (token != NULL) {
//             trainingsample[i] = strtof(token, NULL);
//             } else {
//             // Lidando com o caso em que não há dados suficientes na linha
//             printf("Erro: Não há dados suficientes para preencher InputData na linha.\n");
//             break;
//             }
//         }

//       // Aqui você pode processar a linha conforme necessário, usando os dados em record

//       // Exemplo: exibindo os dados da linha

//     //    trainingsample[0] = 5.1;
//     //    trainingsample[1] = 3.5;
//     //    trainingsample[2] = 1.4;
//     //    trainingsample[3] = 0.2;
//     //    trainingsample[4] = 1;
//     //    trainingsample[5] = 0;
//     //    trainingsample[6] = 0;

//       //printf("InputData: [%.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f]\n", trainingsample[0], trainingsample[1], trainingsample[2], trainingsample[3], trainingsample[4], trainingsample[5], trainingsample[6]);

//       //set the input data on inputdata vector
//       currentneuron =  neuralnetwork->firstlayer->firstneuron;
//       for (int i = 0; i < InputNodes; i++) {
//         currentneuron -> activationfunctionvalue = trainingsample[i];
    
//         currentneuron = currentneuron -> nextneuron;
//       }

//       //set the label on label vector
//       for (int i = InputNodes; i < InputNodes + OutputNodes; i++) {
//         label[i - InputNodes] = trainingsample[i];

//       }


//       currentlayer = currentlayer->nextlayer;
//       //printf("FEEDFOWARD\n");

//       //FEEDFOWARD
//       for (int layer = 1; layer < neuralnetwork -> layers; layer++) {

//           Neuron *previousneuron = currentlayer->previouslayer->firstneuron;
//           currentneuron = currentlayer -> firstneuron;
//           float Z = 0;

//           //printf("neurons %d\n",currentlayer->neurons);

//           for (int i = 0; i < currentlayer -> neurons; i++) {
//             currentweight= currentneuron->firstweight;
//             //printf("wieghts %d\n",currentneuron->weights);
//             for (int j = 0;  j< currentneuron -> weights; j++) {
//               Z += currentweight->weight * previousneuron->activationfunctionvalue;
//               //printf("W %.2f IN %.2f Z %.2f - ",currentweight->weight,previousneuron->activationfunctionvalue,Z);
//               currentweight =currentweight->nextweight;
//               previousneuron = previousneuron ->nextneuron;

//             }
//             currentneuron -> activationfunctionvalue = Sigmoid(Z);
//             //printf("ATIVACAO %.2f\n",currentneuron -> activationfunctionvalue);
//             Z = 0;
//             currentneuron = currentneuron -> nextneuron;
//             previousneuron = currentlayer->previouslayer->firstneuron;
//           }

//         currentlayer = currentlayer -> nextlayer;
//       }


//       //End FeedFoward

//       currentneuron = neuralnetwork -> lastlayer -> firstneuron;
//       //getting the neurons output value for loss function
//       for (int i = 0; i < OutputNodes; i++) {
//         outputdata[i] = currentneuron -> activationfunctionvalue;
//         currentneuron = currentneuron -> nextneuron;
//       }
//       //calculatting the error
//       //Error = CategoricalCrossEntropy(label, outputdata, OutputNodes);
//       //printf("Loss: %f\n", Error);



//       //printf("BACKPROPAGATION\n");


//       //BACKPROPAGATION

//       currentlayer = neuralnetwork -> lastlayer;

//       for (int layer = neuralnetwork -> layers ; layer > 1; layer--) {

//         //verify the last layer
//         if (layer == neuralnetwork -> layers) {
//           //printf("camada %d \n",layer);
          
//           float neuronoutputvalue=0;
//           int labelposition = 0;
//           currentneuron = currentlayer -> firstneuron;

//           //getting the correct position associated to the label
//           for (int i = 0; i < OutputNodes; i++) {
//             if (label[i] == 1) {
//               labelposition = i;
//               break;
//             }
//           }

//           //getting the neuron output where the label position is equal to 1
//           for (int i = 0; i < currentlayer -> neurons; i++) {
//             if (i == labelposition) {
//               neuronoutputvalue = currentneuron -> activationfunctionvalue;
//               break;
//             }
//             currentneuron = currentneuron -> nextneuron;
//           }
//           //printf("%.2f\n",neuronoutputvalue);

//           //allocate memory for delta functions
//           deltafunctionsnumber = 1;
//           deltafunctions = (float * ) malloc(deltafunctionsnumber * sizeof(float));
//           deltafunctions[0] = CategoricalCrossEntropyDerivative(label[labelposition], neuronoutputvalue) * SigmoidDerivative(neuronoutputvalue);
//          // printf("delta %f\n", deltafunctions[0]);

//           //alocate memory for weights matrix
//           weightslines = currentlayer ->previouslayer -> neurons;
//           weightscolumn = 1;

//           weights = (float ** ) malloc(weightslines * sizeof(float * ));

//           for (int i = 0; i < currentlayer -> previouslayer -> neurons; i++) {
//             weights[i] = (float * ) malloc(weightscolumn * sizeof(float));
//           }

//           currentneuron = currentlayer -> firstneuron;
//           Neuron* previousneuron; 
          
//           for (int i = 0; i < currentlayer->neurons; i++)
//           {
//             if(i == labelposition){

//               previousneuron = currentlayer->previouslayer->firstneuron;
//               currentweight = currentneuron->firstweight;
//               for(int j=0;j<currentlayer->previouslayer->neurons ;j++){
//                 //printf("W %f A %f ",currentweight->weight,previousneuron->activationfunctionvalue);
//                 currentweight->weight +=  - deltafunctions[0] * previousneuron->activationfunctionvalue * LearningRate; 
//                 //printf("D %f Wn %f\n",deltafunctions[0],currentweight->weight);
//                 weights[j][0] = currentweight -> weight;
//                 previousneuron =previousneuron->nextneuron;
//                 currentweight = currentweight->nextweight;
//               }
//             }

//           }

//         } 
//         else{

//           //printf("camada %d \n",layer);

//           float * deltafunctionsaux = (float * ) malloc(deltafunctionsnumber * sizeof(float));
//           memcpy(deltafunctionsaux, deltafunctions, deltafunctionsnumber * sizeof(int));
//           deltafunctions = (float * ) realloc(deltafunctions, currentlayer->neurons * sizeof(int));
//           float deltafunctionaccumulation;

//           //printf("DELTAS 1\n");

//           currentneuron = currentlayer->firstneuron;
//           //Neuron * nextneuron = currentlayer->nextlayer->firstneuron;
//           for (int i = 0; i < weightslines; i++) {
//             deltafunctionaccumulation = 0;
//             for (int j = 0; j < weightscolumn; j++) {
//              // printf("Da%d %f * W %f ",j, deltafunctionsaux[j], weights[i][j]);
//               deltafunctionaccumulation += deltafunctionsaux[j] * weights[i][j];
//             }
          
//             deltafunctions[i] = deltafunctionaccumulation * SigmoidDerivative(currentneuron->activationfunctionvalue);
//            // printf("A %f Dn%i %f\n",currentneuron->activationfunctionvalue,i, deltafunctions[i]);
            
//             currentneuron = currentneuron->nextneuron;
//           }



//           weightscolumn = weightslines;
//           weightslines = currentlayer -> previouslayer -> neurons;
//           deltafunctionsnumber = weightslines;
//           //printf("wl %d wc %d\n",weightslines,weightscolumn);
//           weights = (float ** ) malloc(weightslines * sizeof(float * ));
          
//           for (int i = 0; i < weightslines; i++) {
//             weights[i] = (float * ) malloc(weightscolumn * sizeof(float));
//           }

          
//           currentneuron = currentlayer->firstneuron;
//           Neuron *previousneuron; 
          
          
//           for(int i = 0; i< currentlayer->neurons;i++){
            
//             currentweight = currentneuron->firstweight;
//             previousneuron = currentlayer->previouslayer->firstneuron;
//             for (int j = 0; j < currentneuron->weights; j++){
//               //printf("W %.2f D %.2f A %.2f GW %f",currentweight->weight,deltafunctions[i],previousneuron->activationfunctionvalue,- deltafunctions[i]*previousneuron->activationfunctionvalue*LearningRate);
//               currentweight->weight += - deltafunctions[i]*previousneuron->activationfunctionvalue*LearningRate;
//               weights[j][i] = currentweight->weight;
//               //printf(" Wn %f ///",currentweight->weight);
//               currentweight = currentweight->nextweight;
//               previousneuron = previousneuron->nextneuron;
//             }
//               //printf("\n");
            
//             currentneuron = currentneuron -> nextneuron;

//           }

//           //printMatriz(weights,weightslines,weightscolumn);


//           if(deltafunctionsaux!=NULL){
//             free(deltafunctionsaux);
//           }
        
//         }

//         currentlayer = currentlayer -> previouslayer;

//       }

//       if(weights!=NULL){
//         free(weights);
//       }
//       if(deltafunctions!=NULL){
//         free(deltafunctions);
//       }

//     //printf("Loss: %f\n",Error);
//     size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_8BIT);
//     printf("Free Heap Size: %u bytes\n", free_heap);


//     }

//     // Fecha o arquivo
//     fclose(file);

//   //}

//     //printf("epoch %d Loss: %f\n",TrainingCycle+1,Error);


// }