idf_component_register(SRCS "iLot.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver lora_communication ra01s crypto pspot)
