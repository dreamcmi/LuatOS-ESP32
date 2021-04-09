
# option(BUILD_SHARED_LIBS OFF)   
# idf_component_register(SRCS "./src"
#                     INCLUDE_DIRS "./include"
#                     REQUIRES lua)
# include_directories(./include)
# aux_source_directory(./modules LUA_SRCS)
# add_library(lua ${LUA_SRCS})

idf_component_register(SRC_DIRS modules INCLUDE_DIRS include ../lua/include )
