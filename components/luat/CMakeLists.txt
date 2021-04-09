option(BUILD_SHARED_LIBS OFF)   
idf_component_register(SRCS "./modules"
                    INCLUDE_DIRS "./include"
                    REQUIRES lua)
# include_directories(./include)
# aux_source_directory(./modules LUAT_SRCS)
# add_library(luat ${LUAT_SRCS})
