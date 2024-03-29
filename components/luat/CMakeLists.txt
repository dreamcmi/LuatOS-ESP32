idf_component_register(
        SRC_DIRS 
                freertos
                port
                ../../../LuatOS/luat/modules 
                ../../../LuatOS/luat/vfs
                ../../../LuatOS/components/lua-cjson
                ../../../LuatOS/components/eink
                ../../../LuatOS/components/epaper
                ../../../LuatOS/components/u8g2
                ../../../LuatOS/components/fatfs
                ../../../LuatOS/components/lfs
                ../../../LuatOS/components/minmea 
                ../../../LuatOS/components/cjson
                ../../../LuatOS/components/coremark
                ../../../LuatOS/components/sfud
                ../../../LuatOS/components/sfd
                ../../../LuatOS/components/flashdb/src
                ../../../LuatOS/components/fal/src
                ../../../LuatOS/components/lcd
                ../../../LuatOS/components/lvgl/gen/lv_core
                ../../../LuatOS/components/lvgl/gen/lv_draw
                ../../../LuatOS/components/lvgl/gen/lv_font
                ../../../LuatOS/components/lvgl/gen/lv_misc
                ../../../LuatOS/components/lvgl/gen/lv_widgets
                ../../../LuatOS/components/lvgl/font
                ../../../LuatOS/components/lvgl/binding
                ../../../LuatOS/components/lvgl/src/lv_core
                ../../../LuatOS/components/lvgl/src/lv_draw
                ../../../LuatOS/components/lvgl/src/lv_font
                ../../../LuatOS/components/lvgl/src/lv_hal
                ../../../LuatOS/components/lvgl/src/lv_misc
                ../../../LuatOS/components/lvgl/src/lv_themes
                ../../../LuatOS/components/lvgl/src/lv_widgets
                ../../../LuatOS/components/lvgl/exts/lv_qrcode
                ../../../LuatOS/components/lvgl/exts/lv_bmp
                ../../../LuatOS/components/lvgl/exts/lv_png
                ../../../LuatOS/components/lvgl/exts/lv_sjpg
                ../../../LuatOS/components/lvgl/lv_demos/src/assets
                ../../../LuatOS/components/lvgl/lv_demos/src/lv_demo_benchmark
                ../../../LuatOS/components/lvgl/lv_demos/src/lv_demo_keypad_encoder
                ../../../LuatOS/components/lvgl/lv_demos/src/lv_demo_music
                ../../../LuatOS/components/lvgl/lv_demos/src/lv_demo_printer
                ../../../LuatOS/components/lvgl/lv_demos/src/lv_demo_stress
                ../../../LuatOS/components/lvgl/lv_demos/src/lv_demo_widgets
                ../../../LuatOS/components/lvgl/lv_demos/src/lv_ex_get_started
                ../../../LuatOS/components/lvgl/lv_demos/src/lv_ex_style
                ../../../LuatOS/components/ymodem
                ../../../LuatOS/components/cmux
                ../../../LuatOS/components/shell
                ../../../LuatOS/components/gtfont
                ../../../LuatOS/components/tjpgd
                ../../../LuatOS/components/qrcode
                ../../../LuatOS/components/luatfonts
                ../../../LuatOS/components/common
                ../../../LuatOS/components/lora
                ../../../LuatOS/components/lora/sx126x
                ../../../LuatOS/components/mlx90640-library
                ../../../LuatOS/components/crypto
                ../../../LuatOS/components/iotauth
                ../../../LuatOS/components/i2c-tools
                ../../../LuatOS/components/serialization/protobuf

        INCLUDE_DIRS include 
                ../../../LuatOS/lua/include 
                ../../../LuatOS/luat/include 
                ../../../LuatOS/components/lua-cjson
                ../../../LuatOS/components/eink
                ../../../LuatOS/components/epaper
                ../../../LuatOS/components/u8g2
                ../../../LuatOS/components/fatfs
                ../../../LuatOS/components/lfs
                ../../../LuatOS/components/minmea
                ../../../LuatOS/components/cjson
                ../../../LuatOS/components/coremark
                ../../../LuatOS/components/sfud
                ../../../LuatOS/components/sfd
                ../../../LuatOS/components/flashdb/inc
                ../../../LuatOS/components/fal/inc
                ../../../LuatOS/components/lcd
                ../../../LuatOS/components/lvgl
                ../../../LuatOS/components/lvgl/src
                ../../../LuatOS/components/lvgl/binding
                ../../../LuatOS/components/lvgl/gen
                ../../../LuatOS/components/lvgl/font
                ../../../LuatOS/components/lvgl/src/lv_font
                ../../../LuatOS/components/lvgl/exts/lv_bmp
                ../../../LuatOS/components/lvgl/exts/lv_sjpg
                ../../../LuatOS/components/lvgl/exts/lv_gif
                ../../../LuatOS/components/lvgl/exts/lv_png
                ../../../LuatOS/components/lvgl/exts/lv_qrcode
                ../../../LuatOS/components/lvgl/lv_demos
                ../../../LuatOS/components/ymodem
                ../../../LuatOS/components/cmux
                ../../../LuatOS/components/shell
                ../../../LuatOS/components/gtfont
                ../../../LuatOS/components/tjpgd
                ../../../LuatOS/components/qrcode
                ../../../LuatOS/components/luatfonts
                ../../../LuatOS/components/common
                ../../../LuatOS/components/lora
                ../../../LuatOS/components/mlx90640-library
                ../../../LuatOS/components/iotauth
                ../../../LuatOS/components/i2c-tools
                ../../../LuatOS/components/serialization/protobuf

        REQUIRES 
                lua 
                freertos 
                spiffs 
                vfs 
                esp_adc_cal 
                bt mbedtls 
                esp_http_client 
                esp_hw_support 
                mqtt 
                sdmmc 
                fatfs 
                led_strip 
                miniz 
                esp_websocket_client 
                libhelix-mp3
)

idf_build_set_property(C_COMPILE_OPTIONS "-D__LUATOS__=1 -Wno-error=unused-const-variable -Wno-error=pointer-sign" APPEND)
