# CMakeLists.txt inside lib/FederatedLearning

idf_component_register(
    SRCS "federatedlearning.c"
    INCLUDE_DIRS "."
    REQUIRES spiffs
)

# Adicione esta linha para tornar o diretório um componente IDF
# set(COMPONENT_ADD_INCLUDEDIRS ".")



