# option(BUILD_SHARED_LIBS OFF)   
# idf_component_register(SRCS "./src"
#                     INCLUDE_DIRS "./include"
#                     REQUIRES lua)
# include_directories(./include)
# aux_source_directory(./src LUA_SRCS)
# add_library(lua ${LUA_SRCS})

# set(COMPONENT_SRCS "./src/*.c" )
# set(COMPONENT_ADD_INCLUDEDIRS "include")
# register_component()

idf_component_register(
    SRC_DIRS ../../../LuatOS/lua/src 
    INCLUDE_DIRS ../../../LuatOS/lua/include 
        ../../../LuatOS/luat/include
        ../luat/include
)
