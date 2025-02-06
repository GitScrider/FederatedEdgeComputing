# Federated Learning Framework for ESP32

## 📌 Introduction

This framework enables federated learning implementation on ESP32 microcontrollers, allowing distributed training with a centralized server. Designed for efficiency, it ensures low-latency communication and optimized energy consumption in edge computing applications.

## 🚀 Key Features

- Fully distributed federated learning support.
- Efficient communication between ESP32 nodes and a central server via HTTP/REST.
- Real-time capture of key metrics, including latency, message exchange, and power consumption.
- Lightweight implementation in C for enhanced performance on embedded systems.
- Configuration of the neural network and federated model hyperparameters.

## 🏗 Federated Learning Architecture

The framework incorporates a federated learning architecture compatible with various models, constrained only by the ESP32’s hardware limitations. The architecture features a central server connected to multiple ESP32 modules (acting as federated nodes) over a Wi-Fi network facilitated by a local router. The system’s structure consists of three interlinked components: `FederatedLearning`, `NodeControl`, and `NeuralNetwork`.

- **FederatedLearning** serves as the central structure, managing the global model’s state while linking the `NeuralNetwork` and `NodeControl` modules.
- **NeuralNetwork** configures and trains deep neural networks. It dynamically constructs neuron layers and weights through pointers, enabling flexible model designs limited only by the device’s memory.
- **NodeControl** handles client nodes within the federated network and aggregates their locally trained models into the global model.

[Federated Learning Struct](ProjectImages/struct.png)

These interconnected structures are dynamically allocated, supporting configurations of arbitrary sizes (limited by the device’s RAM). The double-linked structure enables efficient feed-forward and back-propagation processes, ensuring the system can accommodate various neural network designs.

Neurons implemented include **input**, **hidden**, and **output** types. Activation functions available for hidden neurons are **ReLU**, **Sigmoid**, and **Perceptron**, while the output neurons support the **Softmax** function, crucial for multi-class classification. The framework provides **categorical cross-entropy** and **mean squared error** loss functions, alongside **Lasso** and **Ridge** regularization methods to mitigate overfitting. This comprehensive functionality facilitates flexible model design and training.


## 📊 How It Works

The server starts by initializing and configuring the Federated Learning (FL) model, dynamically constructing the following structures:

- **FederatedLearning**
- **NodeControl**
- **NeuralNetwork**

The configuration parameters include:

- Number of client nodes
- Federated rounds
- Neural network layers
- Activation functions
- Regularization methods
- Training epochs

Once initialized, the server launches both HTTP and WebSocket services in separate threads, enabling client nodes to register and interact with the global model. Here's a high-level overview of the process:

1. **Initialization**:
   - The server configures the FL model, including neural network layers, activation functions, regularization, and training settings.

2. **Client Registration**:
   - Client nodes register and begin interaction with the server's global model.

3. **Model Distribution**:
   - After all client nodes are registered, the server distributes the global model to the clients.

4. **Model Training**:
   - Each client node performs local training on its data.

5. **Model Aggregation**:
   - Using **Federated Averaging (FedAvg)**, the server aggregates the locally trained models from each client to update the global model. The server maintains backups to ensure resilience.

6. **Evaluation**:
   - After each federated round, the server evaluates the updated global model.

7. **Federated Rounds**:
   - The process repeats for the predefined number of federated rounds.

8. **Final Evaluation**:
   - After completing all rounds, the server performs a final evaluation of the global model and assesses its performance metrics.

[Client and Server System FLow](ProjectImages/systemflow.png)


## 🏗 Repository Structure

### 1. **`FederatedLearningServer` Folder (Server)**
   - The server is developed in C and initially configured for a Linux environment.
   - **Initialization**: The server is started via a Makefile using the `make all` command.
   - **Main Files**:
     - **`main` Folder**:
       - Contains the initialization of the federated model, the HTTP server thread, and the WebSocket server for communication with the client.
     - **`fedlearning.json`**: Example of the federated model in JSON format.
     - **`datasetevaluation.csv`**: Contains 20% of the dataset reserved for model evaluation, located in the `data` folder.
   - **`lib` Folder**: Contains the headers for files found in the `src` folder.
   - **`src` Folder**: Contains the following files:
     - `JSONConverter.c`: Converts the model from C structure to JSON and vice versa.
     - `cJSON.c`: JSON conversion library.
     - `httphandlers.c`: Contains the HTTP endpoints.
     - `httpserver.c`: HTTP server configuration.
     - `websockethandlers.c`: Contains the WebSocket endpoint.
     - `websocketserver.c`: WebSocket server configuration.
     - `federatedlearning.c`: Configuration for the federated training model administration and evaluation.
       - Example of the function `setFederatedLearningGlobalModel()` for setting federated neural network hyperparameters:

         ```c
         void setFederatedLearningGlobalModel() {
             FederatedLearning *federatedLearningInstance = getFederatedLearningInstance();
             // Code to configure neural network parameters...
             printf("Neural Network Compiled!\n");
         }
         ```

### 2. **`FederatedLearningESP32` Folder (Client)**
   - The client is developed on the ESP-IDF platform.
   - **`partitions.csv` File**: Configures the ESP32 partitions based on the available flash memory on the target device.
   - **`main` Folder**:
     - Contains the `espconfiguration.c` file for configuring the SPIFFS filesystem, input/output pins, UART bus, and Wi-Fi.
     - The header file allows configuration of Wi-Fi pins, SSID, and password.
   - **`data` Folder**:
     - **`dataset.csv`**: Iris flower dataset with 120 examples for training.
     - **`dataset1.csv`, `dataset2.csv`, `dataset3.csv`**: Contain the same examples as `dataset.csv`, but divided into 40 examples for testing federated training.
   - **`Compoenents` Folder**:
      - **`cJSON` Folder**: Files responsible for converting the model between JSON and C structure.
      - **`espwebsocketclient` Folder**: Files for WebSocket configuration for the client.
      - **`federatedlearning` Folder**: Files for federated training model configuration.
      - The file is read from the ESP32 `store` partition in line 612 of `federatedlearning.c`:
         ```c
         file = fopen("/storage/dataset.csv", "r");
         ```
      - **`http` Folder**: Contains the configurations for the server's HTTP endpoints.
      - **`websocket` Folder**: Contains the configuration to open a WebSocket connection and send the model to the server.


## 🛠️ Setup

### 1️⃣ Requirements

- ESP32 Dev Kit
- Raspberry Pi (or another Linux device) as the central server
- ESP-IDF for firmware compilation
- Install de CP210X driver in **`CP210X driver` Folder** for the microcontroller flash comunication (If Necessary)

### 2️⃣ Server Setup

1. Navigate to the server directory and compile:
   ```sh
   cd FederatedLearningServer/
   make build
   ```
2. Run the server:
   ```sh
   make run
   ```
3. To verify server-client communication, send a test request using Postman or curl:

```sh
curl -X POST http://<SERVER_IP>:8888/api/testpost \
     -H "Content-Type: application/json" \
     -d '{"key": "test", "value": 1}'
```

### 3️⃣ ESP32 Setup

1. Install ESP-IDF. 
    - **`Windows`**: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/windows-setup.html
   - **`Linux`**: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html

2. Configure Wi-Fi credentials in `config.h`:
   ```c
   #define WIFI_SSID "Your_SSID"
   #define WIFI_PASSWORD "Your_Password"
   ```
3. Navigate to the ESP32 firmware directory:
   ```sh
   cd FederatedLearningESP32/
   ```
4. Compile and flash the firmware:
   ```sh
   idf.py build flash monitor
   ```

5. Hardware configuration (optional)

For debugging and monitoring, a button and three LEDs were integrated into the system in config.h:
--> Green LED Blinks during initialization and stays on during operation.
--> Button: Initiates the application after initial configuration, streamlining user interaction.
--> Blue LED: Indicates communication with the server via HTTP or WebSocket.
--> Red LED: Signals errors (e.g., initialization, communication, or training failure).

   ```c
   #define LED_PIN_ERROR "12"
   #define LED_PIN_WORKING "14"
   #define LED_PIN_SYNC "27"
   #define BUTTON_PIN "26"
   ```
[ESP32 Hardware Setup](ProjectImages/hardwaresetup.png)


## 🤝 Contributions

We welcome contributions! If you’d like to suggest improvements, please open an issue or submit a pull request.

