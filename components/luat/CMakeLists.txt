idf_component_register(
    SRC_DIRS modules freertos port packages/lua-cjson 
    INCLUDE_DIRS include ../lua/include  packages/lua-cjson
    REQUIRES lua freertos spiffs vfs esp_adc_cal bt mbedtls)
