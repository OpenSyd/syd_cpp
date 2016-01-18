
#----------------------------------------------------------
# libCore: main library
add_subdirectory(${SYD_SOURCE_DIR}/src/core)
#----------------------------------------------------------


#----------------------------------------------------------
# Common database
include_directories(${SYD_SOURCE_DIR}/src/core)
add_subdirectory(${SYD_SOURCE_DIR}/src/common_db)

#----------------------------------------------------------


#----------------------------------------------------------
# Standard Database
include_directories(${SYD_SOURCE_DIR}/src/common_db)
add_subdirectory(${SYD_SOURCE_DIR}/src/std_db)
add_subdirectory(${SYD_SOURCE_DIR}/src/ext)
#----------------------------------------------------------


set(SYD_INCLUDE_FILES ${SYD_INCLUDE_FILES} PARENT_SCOPE)
set(SYD_INCLUDE_FILES ${SYD_LIB_FILES} PARENT_SCOPE)
set(SYD_INCLUDE_FILES ${SYD_BIN_FILES} PARENT_SCOPE)
