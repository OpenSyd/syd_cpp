

#----------------------------------------------------------
# libCore: main library
add_subdirectory(${SYD_SOURCE_DIR}/src/core)
#----------------------------------------------------------


#----------------------------------------------------------
# Common database
include_directories(${SYD_SOURCE_DIR}/src/core)
add_subdirectory(${SYD_SOURCE_DIR}/src/common_db)
# #----------------------------------------------------------


#----------------------------------------------------------
# Standard Database
include_directories(${SYD_SOURCE_DIR}/src/common_db)
add_subdirectory(${SYD_SOURCE_DIR}/src/std_db)
add_subdirectory(${SYD_SOURCE_DIR}/src/ext)
#----------------------------------------------------------
