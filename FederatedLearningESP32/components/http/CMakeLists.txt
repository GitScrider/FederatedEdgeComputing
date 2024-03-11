# CMakeLists.txt inside lib/FederatedLearning

idf_component_register(
    SRCS "httpclient.c"
    INCLUDE_DIRS "."
    REQUIRES esp_http_client federatedlearning cJSON
)

# Adicione esta linha para tornar o diretório um componente IDF
# set(COMPONENT_ADD_INCLUDEDIRS ".")
