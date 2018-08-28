# This file sets up include directories, link directories, and
# compiler settings for a project to use SYD.  It should not be
# included directly, but rather through the SYD_USE_FILE setting
# obtained from SYDConfig.cmake.

#----------------------------------------------------------
# Helping macro
macro(DD in)
    message(${in}=${${in}})
endmacro(DD)
#----------------------------------------------------------


#----------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)
#----------------------------------------------------------


#----------------------------------------------------------
# Find ITK
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
message(STATUS "ITK is found")
#----------------------------------------------------------


#----------------------------------------------------------
# Find Boost
set(Boost_USE_STATIC_LIBS       ON) # only find static libs
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
#set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost REQUIRED COMPONENTS date_time system filesystem)
include_directories( ${Boost_INCLUDE_DIR} )
#----------------------------------------------------------


#----------------------------------------------------------
# Eigen3 (needed by ceres)
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN_INCLUDE_DIR})
include_directories(${EIGEN3_INCLUDE_DIR})
if (NOT EIGEN_INCLUDE_DIR)
  set(EIGEN_INCLUDE_DIR ${EIGEN3_INCLUDE_DIR})
endif()
#----------------------------------------------------------


#----------------------------------------------------------
# Find ceres
find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})
#----------------------------------------------------------


#----------------------------------------------------------
# Find SQlite3 (for odb)
cmake_policy(SET CMP0054 NEW)
find_package(sqlite3)
#----------------------------------------------------------

#----------------------------------------------------------
# Find Odb
find_package(ODB REQUIRED COMPONENTS sqlite)
include(${ODB_USE_FILE})
#----------------------------------------------------------


#----------------------------------------------------------
# Find Gengetopt
find_package(Gengetopt)
#----------------------------------------------------------


#----------------------------------------------------------
# optional plot python
OPTION(SYD_PLOT_MODULE "Plot module (require Python)." OFF)
IF(SYD_PLOT_MODULE)
  find_package(PythonLibs 3 REQUIRED)
  include_directories(${PYTHON_INCLUDE_DIR})
ENDIF(SYD_PLOT_MODULE)
#----------------------------------------------------------


#----------------------------------------------------------
# Add include directories needed to use SYD.
include_directories(BEFORE ${SYD_INCLUDE_DIRS})

#----------------------------------------------------------
# Add link directories needed to use SYD.
link_directories(${SYD_LIBRARY_DIRS})

#----------------------------------------------------------
# To compile odb files // FIXME use include (?)
macro(WRAP_ODB ODB_SRCS)
  foreach(ODB_FILES ${ARGN})
    get_filename_component(ODB_BASEFILENAME ${ODB_FILES} NAME_WE)
    get_filename_component(ODB_FILES_ABS ${ODB_FILES} ABSOLUTE)
    set(ODB_CXX ${ODB_BASEFILENAME}-odb.cxx)
    set(ODB_SCHEMA_CXX ${ODB_BASEFILENAME}-schema.cxx)
    set(ODB_IXX ${ODB_BASEFILENAME}-odb.ixx)
    set(ODB_HXX ${ODB_BASEFILENAME}-odb.hxx)
    set(ODB_OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/${ODB_CXX}
      ${CMAKE_CURRENT_BINARY_DIR}/${ODB_SCHEMA_CXX}
      ${CMAKE_CURRENT_BINARY_DIR}/${ODB_IXX}
      ${CMAKE_CURRENT_BINARY_DIR}/${ODB_HXX})
    if(EXISTS ${ODB_FILES_ABS})
      add_custom_command(OUTPUT ${ODB_OUTPUT}
        COMMAND ${ODB_EXECUTABLE}
        ARGS --std c++11 --database sqlite -I${Boost_INCLUDE_DIR} -I${EIGEN_INCLUDE_DIR} -I${SYD_SOURCE_DIR}/src/std_db -I${SYD_SOURCE_DIR}/src/core  -I${SYD_SOURCE_DIR}/src/common_db  -I${SYD_SOURCE_DIR}/src/ext  --generate-schema --schema-format separate  --generate-query --sqlite-override-null --schema-name ${SCHEMA_NAME} ${ODB_FILES_ABS}
        DEPENDS ${ODB_FILES_ABS})
    else()
      message(FATAL_ERROR "Error odb cannot file the following file: " ${ODB_FILES_ABS})
    endif()
    set(${ODB_SRCS} ${${ODB_SRCS}} ${ODB_OUTPUT})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  endforeach(ODB_FILES)
  set_source_files_properties(${${ODB_SRCS}} PROPERTIES GENERATED TRUE)
endmacro(WRAP_ODB)
#----------------------------------------------------------


#----------------------------------------------------------
# Need thoses directories to include odb generated files
include_directories(${SYD_BINARY_DIR}/src/)
include_directories(${SYD_BINARY_DIR}/src/common_db)
include_directories(${SYD_BINARY_DIR}/src/core)
include_directories(${SYD_BINARY_DIR}/src/std_db)
include_directories(${SYD_BINARY_DIR}/src/plot)
include_directories(${SYD_BINARY_DIR}/src/ext)
# ----------------------------------------------------------

set(SYD_COMMON_DB_LIBRARY sydCommonDatabase ${SYD_CORE_LIBRARIES})
set(SYD_STD_DB_LIBRARY sydStandardDatabase ${SYD_COMMON_DB_LIBRARY})
