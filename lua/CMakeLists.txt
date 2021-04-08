include_directories(./include)
include_directories(../luat/include)
aux_source_directory(./src LUA_SRCS)
add_library(lua ${LUA_SRCS})