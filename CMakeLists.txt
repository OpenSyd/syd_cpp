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
# activate c++0x or c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive")
#----------------------------------------------------------


#----------------------------------------------------------
# cmake include
include(${SYD_SOURCE_DIR}/cmake/common.cmake)
include(${SYD_SOURCE_DIR}/cmake/dependencies.cmake)
include(${SYD_SOURCE_DIR}/cmake/ODB.cmake)


find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

set(CLITK_INCLUDE_DIRS "Indicate clitk src folder" CACHE PATH "clitk include")
set(CLITK_LIBRARY_DIRS "Indicate clitk build folder" CACHE PATH "clitk lib")

set(CLITK_LIBRARY_DIRS ${CLITK_LIBRARY_DIRS}/lib)

set(CLITK_INCLUDE_DIRS_COMMON ${CLITK_INCLUDE_DIRS}/common)
INCLUDE_DIRECTORIES(BEFORE ${CLITK_INCLUDE_DIRS_COMMON})

set(CLITK_INCLUDE_DIRS_ITK ${CLITK_INCLUDE_DIRS}/itk)
INCLUDE_DIRECTORIES(BEFORE ${CLITK_INCLUDE_DIRS_ITK})

set(CLITK_INCLUDE_DIRS_TOOLS ${CLITK_INCLUDE_DIRS}/tools)
INCLUDE_DIRECTORIES(BEFORE ${CLITK_INCLUDE_DIRS_TOOLS})


LINK_DIRECTORIES(${CLITK_LIBRARY_DIRS})

#----------------------------------------------------------


# ----------------------------------------------------------
# Select what is compiled
add_subdirectory(${SYD_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
#----------------------------------------------------------
