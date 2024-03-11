# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(FederatedLearningESP32)

idf_build_set_property(COMPILE_OPTIONS "-Wno-error" APPEND)


list(APPEND EXTRA_COMPONENT_DIRS "components/federatedfearning")
list(APPEND EXTRA_COMPONENT_DIRS "components/http")
list(APPEND EXTRA_COMPONENT_DIRS "components/CJSON")
list(APPEND EXTRA_COMPONENT_DIRS "components/websocket")
list(APPEND EXTRA_COMPONENT_DIRS "components/espwebsocketclient")
