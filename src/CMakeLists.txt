

#----------------------------------------------------------
# libCore: main library
add_subdirectory(${SYD_SOURCE_DIR}/src/core)
#----------------------------------------------------------


#----------------------------------------------------------
# Common command line tools
include_directories(${SYD_SOURCE_DIR}/src/core)
add_subdirectory(${SYD_SOURCE_DIR}/src/tools)
#----------------------------------------------------------


#----------------------------------------------------------
# Standard Database
add_subdirectory(${SYD_SOURCE_DIR}/src/std_db)
#----------------------------------------------------------
