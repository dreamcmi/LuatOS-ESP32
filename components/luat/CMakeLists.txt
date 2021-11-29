
idf_component_register(
    SRC_DIRS ../../../LuatOS/luat/modules 
             freertos
             port
             ../../../LuatOS/luat/packages/lua-cjson
             ../../../LuatOS/luat/vfs 
    INCLUDE_DIRS include 
            ../../../LuatOS/lua/include 
            ../../../LuatOS/luat/include 
            ../../../LuatOS/luat/packages/lua-cjson

    REQUIRES lua freertos spiffs vfs esp_adc_cal bt mbedtls
)
