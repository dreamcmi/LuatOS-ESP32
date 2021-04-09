option(BUILD_SHARED_LIBS OFF)   
idf_component_register(SRCS "./src"
                    INCLUDE_DIRS "./include"
                    REQUIRES luat)
# include_directories(./include)
# aux_source_directory(./src LUA_SRCS)
# add_library(lua ${LUA_SRCS})
