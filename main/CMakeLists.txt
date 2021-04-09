# include_directories(../components/lua/include)
# include_directories(../components/luat/include)
idf_component_register(SRCS 
                    "luatos_main.c" 
                    INCLUDE_DIRS "."
                    )
                     
# add_subdirectory(../components/lua)
# add_subdirectory(../components/luat)
# target_link_libraries(${COMPONENT_LIB} PUBLIC lua luat)
