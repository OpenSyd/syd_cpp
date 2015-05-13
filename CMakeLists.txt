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
# define the set of libraries (needed in test folder)
set(SYD_LIBS
  ${DCMTK_LIBRARIES}
  sydCore
  sydCommonSchema
  odb-sqlite odb sqlite3 z
  ${CERES_LIBRARIES}
  ${ITK_LIBRARIES})
#----------------------------------------------------------


#----------------------------------------------------------
# cmake include
include(${SYD_SOURCE_DIR}/cmake/common.cmake)
include(${SYD_SOURCE_DIR}/cmake/dependencies.cmake)
include(${SYD_SOURCE_DIR}/cmake/ODB.cmake)
#----------------------------------------------------------


#----------------------------------------------------------
# ceres
find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})
#----------------------------------------------------------


#----------------------------------------------------------
# itk
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
#----------------------------------------------------------


#----------------------------------------------------------
# dcmtk
find_package(DCMTK REQUIRED)
include_directories(${DCMTK_INCLUDE_DIRS})
#----------------------------------------------------------


#----------------------------------------------------------
## clitk : needed ?
# set(CLITK_INCLUDE_DIRS "Indicate clitk src folder" CACHE PATH "clitk include")
# set(CLITK_LIBRARY_DIRS "Indicate clitk build folder" CACHE PATH "clitk lib")

# set(CLITK_LIBRARY_DIRS ${CLITK_LIBRARY_DIRS}/lib)

# set(CLITK_INCLUDE_DIRS_COMMON ${CLITK_INCLUDE_DIRS}/common)
# include_directories(BEFORE ${CLITK_INCLUDE_DIRS_COMMON})

# set(CLITK_INCLUDE_DIRS_ITK ${CLITK_INCLUDE_DIRS}/itk)
# include_directories(BEFORE ${CLITK_INCLUDE_DIRS_ITK})


#----------------------------------------------------------
include(CTest)
if(BUILD_TESTING)
  enable_testing()
endif(BUILD_TESTING)
if(BUILD_TESTING)
  add_subdirectory(test)
endif(BUILD_TESTING)
#----------------------------------------------------------


# ----------------------------------------------------------
# Select what is compiled
add_subdirectory(${SYD_SOURCE_DIR}/src ${PROJECT_BINARY_DIR}/src)
#----------------------------------------------------------


# #----------------------------------------------------------
# enable_testing()
# add_test(
#    NAME test_Dump
#    WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test/test_Dump
#    COMMAND test_Dump.sh)
# #----------------------------------------------------------

# #----------------------------------------------------------
# enable_testing()
# add_test(
#    NAME test_InsertInjection
#    WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test/test_InsertInjection
#    COMMAND ../run_test.sh test_InsertInjection.sh)
# #----------------------------------------------------------
