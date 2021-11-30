


idf_component_register(
    SRC_DIRS ../../../LuatOS/luat/modules 
             freertos
             port
             ../../../LuatOS/luat/packages/lua-cjson
             ../../../LuatOS/luat/vfs
             ../../../LuatOS/luat/packages/eink
             ../../../LuatOS/luat/packages/epaper
             ../../../LuatOS/luat/packages/qrcode
             ../../../LuatOS/luat/packages/u8g2
    INCLUDE_DIRS include 
            ../../../LuatOS/lua/include 
            ../../../LuatOS/luat/include 
            ../../../LuatOS/luat/packages/lua-cjson
            ../../../LuatOS/luat/packages/eink
            ../../../LuatOS/luat/packages/epaper
            ../../../LuatOS/luat/packages/qrcode
            ../../../LuatOS/luat/packages/u8g2

    REQUIRES lua freertos spiffs vfs esp_adc_cal bt mbedtls
)

idf_build_set_property(C_COMPILE_OPTIONS "-D__LUATOS__=1 -Wno-error=unused-const-variable" APPEND)
