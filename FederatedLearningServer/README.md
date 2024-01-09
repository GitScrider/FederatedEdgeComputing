# FederatedLearningServer


This is the Federated Server responsible for merge and distribute the global model for the ESP32 Clients for the training.

The directory src have the necessary files and the lib directory is the .h correlated.

The Makefile is configured as follows:
    * make: compile and generated the program 
    * make clean: force remove of .o files inside the .build directory and the program file
    * make run: execute the program file