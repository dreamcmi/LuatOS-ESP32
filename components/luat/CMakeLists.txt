idf_component_register(
    SRC_DIRS modules freertos port packages 
    INCLUDE_DIRS include ../lua/include  
    REQUIRES lua freertos spiffs vfs esp_adc_cal bt)
