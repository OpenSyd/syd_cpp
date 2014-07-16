# ----------------------------------------------------------
# SYD
# ----------------------------------------------------------
project(syd)

cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if(NOT DEFINED SYD_SOURCE_DIR)
  set(SYD_SOURCE_DIR ${PROJECT_SOURCE_DIR})
endif(NOT DEFINED SYD_SOURCE_DIR)
#----------------------------------------------------------


# ----------------------------------------------------------
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
#----------------------------------------------------------


# ----------------------------------------------------------
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /bigobj")
endif(MSVC)
#----------------------------------------------------------


#----------------------------------------------------------
# cmake include
include(${SYD_SOURCE_DIR}/cmake/common.cmake)
include(${SYD_SOURCE_DIR}/cmake/dependencies.cmake)
include(${SYD_SOURCE_DIR}/cmake/ODB.cmake)
#----------------------------------------------------------


# ----------------------------------------------------------
# Select what is compiled
add_subdirectory(${SYD_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
#----------------------------------------------------------
