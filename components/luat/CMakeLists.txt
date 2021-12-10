idf_component_register(
    SRC_DIRS ../../../LuatOS/luat/modules 
             freertos
             port
             ../../../LuatOS/luat/vfs
             ../../../LuatOS/luat/weak
             ../../../LuatOS/luat/packages/lua-cjson
             ../../../LuatOS/luat/packages/eink
             ../../../LuatOS/luat/packages/epaper
             ../../../LuatOS/luat/packages/qrcode
             ../../../LuatOS/luat/packages/u8g2
             ../../../LuatOS/luat/packages/lfs
             ../../../LuatOS/components/coremark
             ../../../LuatOS/components/sfud
             ../../../LuatOS/components/lcd
    INCLUDE_DIRS include 
            ../../../LuatOS/lua/include 
            ../../../LuatOS/luat/include 
            ../../../LuatOS/luat/packages/lua-cjson
            ../../../LuatOS/luat/packages/eink
            ../../../LuatOS/luat/packages/epaper
            ../../../LuatOS/luat/packages/qrcode
            ../../../LuatOS/luat/packages/u8g2
            ../../../LuatOS/luat/packages/fatfs
            ../../../LuatOS/luat/packages/lfs
            ../../../LuatOS/components/coremark
            ../../../LuatOS/components/sfud
            ../../../LuatOS/components/lcd
    REQUIRES lua freertos spiffs vfs esp_adc_cal bt mbedtls
)

idf_build_set_property(C_COMPILE_OPTIONS "-D__LUATOS__=1 -Wno-error=unused-const-variable -Wno-error=pointer-sign" APPEND)
