# CMakeLists.txt inside lib/FederatedLearning

idf_component_register(
    SRCS "cJSON.c" "JSONConverter"
    INCLUDE_DIRS "."
)

# Adicione esta linha para tornar o diretório um componente IDF
set(COMPONENT_ADD_INCLUDEDIRS ".")
