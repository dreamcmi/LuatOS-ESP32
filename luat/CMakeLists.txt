include_directories(./include)
include_directories(../lua/include)
add_library(luat ./modules/luat_main.c 
                 ./modules/luat_luat_bin.c
                 ./modules/luat_lib_rtos.c
                 ./modules/luat_lib_timer.c
                 ./modules/luat_lib_log.c
                 ./modules/luat_lib_pack.c
                 ./modules/luat_lib_zbuff.c
                 ./modules/luat_lib_mqttcore.c
                 ./modules/luat_lib_libcoap.c
                 ./modules/luat_lib_crypto.c
                 ./modules/crc.c
            )
