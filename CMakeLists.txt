# ----------------------------------------------------------
# SYD
# ----------------------------------------------------------
project(syd)


#----------------------------------------------------------
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# Policy for shared library on OSX
cmake_policy(SET CMP0042 NEW) # CMake 3.0

if(NOT DEFINED SYD_SOURCE_DIR)
  set(SYD_SOURCE_DIR ${PROJECT_SOURCE_DIR})
endif(NOT DEFINED SYD_SOURCE_DIR)
#----------------------------------------------------------


#----------------------------------------------------------
## This is required on some system (linux) to allow linking
if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC"  )
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC"  )
endif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
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
#----------------------------------------------------------


#----------------------------------------------------------
# define the set of libraries (needed in test folder)
set(SYD_LIBS
  ${DCMTK_LIBRARIES}
  sydCore
  sydCommonSchema
  odb-sqlite odb sqlite3 z
  ${ITK_LIBRARIES})
#----------------------------------------------------------


# ----------------------------------------------------------
# Include main sources
add_subdirectory(${SYD_SOURCE_DIR}/src)
#----------------------------------------------------------


#----------------------------------------------------------
# tests
include(CTest)
if(BUILD_TESTING)
  enable_testing()
endif(BUILD_TESTING)
if(BUILD_TESTING)
  add_subdirectory(test)
endif(BUILD_TESTING)
#----------------------------------------------------------
