#include "../lib/federatedlearning.h"
#include "../lib/JSONConverter.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

static pthread_mutex_t singletonMutex = PTHREAD_MUTEX_INITIALIZER;

//////////////////////////////////////////////////PRINT//////////////////////////////////////////////////

void PrintNeuralNeuralNetwork(NeuralNetwork * neuralnetwork) {

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

//////////////////////////////////////////////////DEEPLEARNINGMATH//////////////////////////////////////////////////

float Perceptron(float Z){
    if(Z>0)
    {
      return 1;
    }
    else{
     return 0; 
    }
}

float ReLU(float Z){
  if(Z>0){

    return Z;
  }else{
    return 0;
  }
}

float Sigmoid(float Z) {
  return 1.0 / (1.0 + exp(-Z));
}

float SoftMax( float Z,float softmaxsum ){
  return exp(Z)/softmaxsum;
}

//////////////////////////////////////////////////DEEPLEARNINGNEURALNETWORK//////////////////////////////////////////////////

float ActivationFunctionCalculaton(NeuralNetwork *neuralnetwork,float Z, int activationfunctiontype){

  Layer *currentlayer;
  Neuron *currentneuron, *previousneuron;
  Weight *currentweight;

  float sum = 0;
  float softmaxsum=0;

//printf("activaiton function type: %d",activationfunctiontype);

switch (activationfunctiontype){
  case 1:
    return Perceptron(Z);
  case 2:
    return ReLU(Z);
  case 3:
    return Sigmoid(Z); 
  case 4:

        currentlayer = neuralnetwork->lastlayer;
        currentneuron = currentlayer -> firstneuron;
        previousneuron = currentlayer->previouslayer->firstneuron;

        for (int i = 0; i < currentlayer -> neurons; i++) {
          currentweight= currentneuron->firstweight;
          //printf("wieghts %d \nB %f - ",currentneuron->weights,currentneuron->bias);
          sum+= currentneuron->bias;
          for (int j = 0;  j< currentneuron -> weights; j++) {
            sum += currentweight->weight * previousneuron->activationfunctionvalue;
            //printf("W %f IN %f - ",currentweight->weight,previousneuron->activationfunctionvalue,Z);
            currentweight =currentweight->nextweight;
            previousneuron = previousneuron ->nextneuron;
          }
          softmaxsum += exp(sum);

          sum = 0;
          currentneuron = currentneuron -> nextneuron;
          previousneuron = currentlayer->previouslayer->firstneuron;
        }
        //printf("Exp Z %f SOFTMAXSUM += %f\n",exp(Z),softmaxsum);

        return SoftMax(Z,softmaxsum);

  default:
    return 0;
  }

}

void FeedFoward(NeuralNetwork * neuralnetwork){

  Layer *currentlayer = neuralnetwork->firstlayer->nextlayer;
  Neuron *currentneuron;
  Weight *currentweight;
  
  //printf("FEEDFOWARD\n");

  for (int layer = 1; layer < neuralnetwork -> layers; layer++) {

    //printf("LAYER %d\n",layer);
            
    Neuron *previousneuron = currentlayer->previouslayer->firstneuron;
    currentneuron = currentlayer -> firstneuron;
    float Z = 0;
          
    //printf("neurons %d\n",currentlayer->neurons);

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
        currentneuron -> activationfunctionvalue =  ActivationFunctionCalculaton(neuralnetwork,Z,currentlayer->activationfunctiontype);
        //printf("Z %f ATIVACAO %f\n",Z,currentneuron -> activationfunctionvalue);
        Z = 0;
        currentneuron = currentneuron -> nextneuron;
        previousneuron = currentlayer->previouslayer->firstneuron;
      }
    currentlayer = currentlayer -> nextlayer;
    }
}

void InitializeNeuralNetWork(NeuralNetwork * neuralnetwork,int layers,LayerConfig *layersconfig, int lossfunctiontype) {

  LayerConfig *currrentlayerconfig = layersconfig->first;
  Layer *currrentlayer;

  //Inicialize a semente do gerador de números aleatórios
  srand((unsigned) time(NULL));

  neuralnetwork->layers = layers;
  neuralnetwork->lossfunctiontype = lossfunctiontype;
  neuralnetwork -> firstlayer = NULL;
  neuralnetwork -> lastlayer = NULL;

  currrentlayerconfig = layersconfig->first;
 
 for (int i = 0; i < neuralnetwork -> layers; i++) {

    Layer * newLayer = (Layer * ) malloc(sizeof(Layer));

    if (newLayer == NULL) {
      fprintf(stderr, "Alocation memory Failure\n");
      exit(1);
    }

    newLayer -> neurons = currrentlayerconfig->neurons;
    newLayer -> activationfunctiontype = currrentlayerconfig->activationfunctiontype;
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
    
    currrentlayer = newLayer;
    
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
        newneuron -> weights = currrentlayer->previouslayer->neurons;
      } 
      else {
        strcpy(newneuron->neurontype, "HIDDEN");
        newneuron -> weights = currrentlayer->previouslayer->neurons;
      }

      //newneuron -> bias = (float)(rand() % 100) / 100.0; 
      newneuron -> bias = 0.0;

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
        
        // Gere um número aleatório entre 0 e 99 e converta para float entre 0 e 1
        newweight -> weight = (float)(rand() % 100) / 100.0; 
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

    currrentlayerconfig = currrentlayerconfig->next;
  }
}

//////////////////////////////////////////////////FEDERATEDLEARNING//////////////////////////////////////////////////

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

void setFederatedLearningGlobalModel() {

  FederatedLearning *federatedLearningInstance = getFederatedLearningInstance();
  LayerConfig *layerconfig0 = malloc(sizeof(LayerConfig));
  LayerConfig *layerconfig1 = malloc(sizeof(LayerConfig));
  LayerConfig *layerconfig2 = malloc(sizeof(LayerConfig));
  LayerConfig *layerconfig3 = malloc(sizeof(LayerConfig));

  layerconfig0->first =layerconfig0;
  layerconfig0->neurons= 4;
  layerconfig0->activationfunctiontype = 0;
  layerconfig0->next = layerconfig1;

  layerconfig1->neurons= 3;
  layerconfig1->activationfunctiontype = RELU;
  layerconfig1->next = layerconfig2;

  layerconfig2->neurons= 3;
  layerconfig2->activationfunctiontype = RELU;
  layerconfig2->next = layerconfig3;

  layerconfig3->neurons= 3;
  layerconfig3->activationfunctiontype = SOFTMAX;
  layerconfig3->next = NULL;

  InitializeNeuralNetWork(federatedLearningInstance->neuralnetwork, 4 ,layerconfig0, CATEGORICAL_CROSS_ENTROPY);
  
  federatedLearningInstance->neuralnetwork->percentualtraining = 120;
  federatedLearningInstance->neuralnetwork->epoch = 100;
  federatedLearningInstance->neuralnetwork->alpha=0.001;
  federatedLearningInstance->neuralnetwork->regularization=L2;
  federatedLearningInstance->neuralnetwork->lambda =0.01;
  federatedLearningInstance->globalmodelstatus=1;

  printf("Neural Network Compiled!\n");

}

void FederationAveraging(NeuralNetwork * globalneuralnetwork, NeuralNetwork * clientneuralnetwork, int currenttrainingcounter, int clienttrainingcounter){

  Layer * currentgloballayer = globalneuralnetwork -> firstlayer;
  Layer * currentclientlayer = clientneuralnetwork -> firstlayer;

  while (currentgloballayer != NULL) {

    Neuron * currentglobalneuron = currentgloballayer -> firstneuron;
    Neuron * currentclientneuron = currentclientlayer -> firstneuron;

    while (currentglobalneuron != NULL) {

      Weight * currentglobalweight = currentglobalneuron -> firstweight;
      Weight * currentclientweight = currentclientneuron -> firstweight;

      while (currentglobalweight != NULL) {

        printf("%.5f * %d + %.5f * %d / %d + %d\n",currentglobalweight->weight,currenttrainingcounter,currentclientweight->weight,clienttrainingcounter,currenttrainingcounter,clienttrainingcounter);

        currentglobalweight->weight = (currentglobalweight->weight*currenttrainingcounter + currentclientweight->weight*clienttrainingcounter)/(currenttrainingcounter+clienttrainingcounter);
        currentglobalweight = currentglobalweight -> nextweight;
        currentclientweight = currentclientweight-> nextweight;
      }
      //printf("\n");
      currentglobalneuron = currentglobalneuron -> nextneuron;
      currentclientneuron = currentclientneuron -> nextneuron;
    }
      //printf("\n");

    currentgloballayer = currentgloballayer -> nextlayer;
    currentclientlayer = currentclientlayer -> nextlayer;
  }
}

void AggregationModel(FederatedLearning * clientmodel){

FederatedLearning * federatedlearninginstance = getFederatedLearningInstance();

PrintNeuralNeuralNetwork(clientmodel->neuralnetwork);

printf("\n\n %d %d \n\n",
                federatedlearninginstance->trainingscounter,
                clientmodel->trainingscounter);

FederationAveraging(federatedlearninginstance->neuralnetwork,
                clientmodel->neuralnetwork,
                federatedlearninginstance->trainingscounter,
                clientmodel->trainingscounter);
                
federatedlearninginstance->trainingscounter += clientmodel->trainingscounter;

}

void PerformanceMetrics(NeuralNetwork * neuralnetwork,int PercentualEvaluation,float Threshold){
  
  float trainingsample[neuralnetwork->firstlayer->neurons + neuralnetwork->lastlayer->neurons];
  float label[neuralnetwork->lastlayer->neurons];
  float Precision=0, Recall=0, Accuracy=0,Specificity=0,F1Score=0;

  int total=0,truepositive=0,falsepositive=0,truenegative=0,falsenegative=0;

  // variables to cycle through pointers
  Layer * currentlayer = neuralnetwork -> firstlayer;
  Neuron * currentneuron = currentlayer -> firstneuron;

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

      FeedFoward(neuralnetwork);

      currentneuron = neuralnetwork->lastlayer->firstneuron;

      for(int i=0;i< neuralnetwork->lastlayer->neurons;i++){
         printf("%f ",currentneuron->activationfunctionvalue);
         printf("%f ",label[i]);
         printf("%f ",Threshold);
        if(label[i]==1){
           //printf(" positivo\n");
          if(currentneuron->activationfunctionvalue > Threshold){
            printf(" truepositive \n");
            truepositive++;
          }else{
            printf(" falsepositive \n");

            falsepositive++;
          }
          total++;
          
        }else{
          //printf("negativo\n");

          if(currentneuron->activationfunctionvalue < Threshold){
            printf(" truenegative \n");
            
            truenegative++;
          }else{
            printf(" falsenegative \n");

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
      printf("Accuracy: %.2f\n",Accuracy);
      printf("Precision: %.2f\n",Precision);
      printf("Recall: %.2f\n",Recall);
      printf("Specificity: %.2f\n",Specificity);
      printf("F1-Score: %.2f\n",F1Score);
}
