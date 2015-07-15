# ---------------------------------------------------------
project(syd)
# ---------------------------------------------------------


#----------------------------------------------------------
# cmake policies
cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# Policy for shared library on OSX
cmake_policy(SET CMP0042 NEW) # CMake 3.0
#----------------------------------------------------------


#----------------------------------------------------------
# Use c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#----------------------------------------------------------


#----------------------------------------------------------
# Basic variables
set(SYD_SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(SYD_BINARY_DIR ${PROJECT_BINARY_DIR})
#----------------------------------------------------------


#----------------------------------------------------------
# cmake include
set(CMAKE_MODULE_PATH "${SYD_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(cmake/dependencies.cmake)
include(cmake/UseSYD.cmake)
#----------------------------------------------------------


#----------------------------------------------------------
## Default to release
## http://cmake.3232098.n2.nabble.com/Default-value-for-CMAKE-BUILD-TYPE-td7550756.html
# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
#----------------------------------------------------------


#----------------------------------------------------------
# Output directories.
IF(NOT LIBRARY_OUTPUT_PATH)
  SET (LIBRARY_OUTPUT_PATH ${SYD_BINARY_DIR}/lib CACHE INTERNAL "Single output directory for building all libraries.")
ENDIF(NOT LIBRARY_OUTPUT_PATH)
IF(NOT EXECUTABLE_OUTPUT_PATH)
  SET (EXECUTABLE_OUTPUT_PATH ${SYD_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
ENDIF(NOT EXECUTABLE_OUTPUT_PATH)
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
SET(CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
#----------------------------------------------------------


#----------------------------------------------------------
SET(SYD_VERSION_MAJOR "0")
SET(SYD_VERSION_MINOR "2")
SET(SYD_VERSION_PATCH "0")
#----------------------------------------------------------


#----------------------------------------------------------
## This is required on some system (linux) to allow linking
if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC"  )
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC"  )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC"  )
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
# define the set of libraries
set(SYD_CORE_LIBRARIES
  ${DCMTK_LIBRARIES}
  sydCore
  sydCommonSchema
  odb-sqlite odb sqlite3 z
  ${Boost_LIBRARIES}
  ${ITK_LIBRARIES})
set(SYD_LIBRARIES
  sydStandardDatabase
  ${SYD_CORE_LIBRARIES})
# ----------------------------------------------------------


# ----------------------------------------------------------
# Include main sources
add_subdirectory(${SYD_SOURCE_DIR}/src)
#----------------------------------------------------------


#----------------------------------------------------------
# Set some variable
SET(SYD_LIBRARY_PATH "${LIBRARY_OUTPUT_PATH}")
SET(SYD_EXECUTABLE_PATH "${EXECUTABLE_OUTPUT_PATH}")

LIST(APPEND SYD_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/core")
LIST(APPEND SYD_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/std_db")
LIST(APPEND SYD_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/ext")
LIST(APPEND SYD_INCLUDE_DIRS "${SYD_BINARY_DIR}/src/core")

# Generate SYDConfig.cmake for the build tree.
SET (SYD_USE_FILE "${SYD_SOURCE_DIR}/cmake/UseSYD.cmake")
SET (SYD_LIBRARY_DIRS ${SYD_LIBRARY_PATH})
CONFIGURE_FILE (cmake/SYDConfig.cmake.in SYDConfig.cmake @ONLY)
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
