idf_component_register(SRCS "espconfiguration.c" "main.c"
                    INCLUDE_DIRS "."

                    
)

spiffs_create_partition_image(storage ../data FLASH_IN_PROJECT)