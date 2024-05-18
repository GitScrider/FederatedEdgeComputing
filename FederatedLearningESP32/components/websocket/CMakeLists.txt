# CMakeLists.txt inside lib/

idf_component_register(
    SRCS  "websocketclient.c"
    INCLUDE_DIRS "."
    REQUIRES esp_event espwebsocketclient cJSON federatedlearning
)

# Adicione esta linha para tornar o diretório um componente IDF
# set(COMPONENT_ADD_INCLUDEDIRS ".")
