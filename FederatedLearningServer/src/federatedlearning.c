#include "../lib/federatedlearning.h"
#include "../lib/JSONConverter.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
      printf("Bias: %.10f\n",currentneuron->bias);

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

//void InitializeNeuralNetWork(NeuralNetwork * neuralnetwork, int * neuronsInEachLayerVector) {
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

void PerformanceMetrics(int PercentualEvaluation,int Threshold){
  
    printf("teste 1\n");


  NeuralNetwork *neuralnetwork = getFederatedLearningInstance()->neuralnetwork;

    printf("teste 2 %d\n",neuralnetwork->firstlayer->neurons);



  float trainingsample[neuralnetwork->firstlayer->neurons + neuralnetwork->lastlayer->neurons];
  float label[neuralnetwork->lastlayer->neurons];
  float outputdata[neuralnetwork->lastlayer->neurons];
  float Precision=0.0, Recall=0.0, Accuracy=0.0,Specificity=0.0,F1Score=0.0;

    printf("teste 3\n");


  int total=0,truepositive=0,falsepositive=0,truenegative=0,falsenegative=0;

  // variables to cycle through pointers
  Layer * currentlayer = neuralnetwork -> firstlayer;
  Neuron * currentneuron = currentlayer -> firstneuron;
  Weight * currentweight;

  FILE * file = NULL;
  char line[1024];
  char filename[] = "data/datasetevaluation.csv";
  //Abre o arquivo CSV para leitura
    printf("Opening CSV file\n");
    file = fopen(filename, "r");

    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }

  for(int count=0;count<PercentualEvaluation;count++){
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

