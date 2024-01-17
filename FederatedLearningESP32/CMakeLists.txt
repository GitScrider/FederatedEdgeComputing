# CMakeLists.txt na raiz do seu projeto

cmake_minimum_required(VERSION 3.16.0)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(FederatedLearningESP32)

# Defina aqui suas variáveis de ambiente, se necessário

# Adicione os componentes extras (lib/FederatedLearning) ao caminho
list(APPEND EXTRA_COMPONENT_DIRS "lib/federatedfearning")
list(APPEND EXTRA_COMPONENT_DIRS "lib/http")
list(APPEND EXTRA_COMPONENT_DIRS "lib/CJSON")

#spiffs_create_partition_image(spiffs ${CMAKE_SOURCE_DIR}/data FLASH_IN_PROJECT)




# Adicione os outros componentes necessários ao caminho
#list(APPEND EXTRA_COMPONENT_DIRS "outros/componentes")

# Defina suas configurações do IDF

# Adicione outras configurações específicas do projeto, se necessário
