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
# Find ITK (required)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
message(STATUS "ITK is found")

#----------------------------------------------------------
# Find Boost (required)
find_package(Boost REQUIRED date_time system filesystem)
include_directories( ${Boost_INCLUDE_DIR} )
message(STATUS "Boost is found")

#----------------------------------------------------------
# Add include directories needed to use SYD.
include_directories(BEFORE ${SYD_INCLUDE_DIRS})

## get list of include for itk (not used yet)
set(I_ITK_INCLUDE "")
foreach(A ${ITK_INCLUDE_DIRS})
  set(I_ITK_INCLUDE ${I_ITK_INCLUDE} "-I" ${A})
endforeach()


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
    set(ODB_IXX ${ODB_BASEFILENAME}-odb.ixx)
    set(ODB_HXX ${ODB_BASEFILENAME}-odb.hxx)
    set(ODB_OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/${ODB_CXX}
      ${CMAKE_CURRENT_BINARY_DIR}/${ODB_IXX}
      ${CMAKE_CURRENT_BINARY_DIR}/${ODB_HXX})
    if(EXISTS ${ODB_FILES_ABS})
      add_custom_command(OUTPUT ${ODB_OUTPUT}
        COMMAND ${ODB_EXECUTABLE}
        ARGS --std c++11 --database sqlite -I${DCMTK_INCLUDE_DIR_BASE} -I${SYD_SOURCE_DIR}/src/std_db -I${SYD_SOURCE_DIR}/src/core  -I${SYD_SOURCE_DIR}/src/common_db  --generate-schema --schema-format embedded --generate-query --sqlite-override-null --schema-name ${SCHEMA_NAME} ${ODB_FILES_ABS}
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
# activate c++0x or c++11
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive")
#----------------------------------------------------------


#----------------------------------------------------------
# Need thoses directories to include odb generated files
include_directories(${SYD_BINARY_DIR}/src/)
include_directories(${SYD_BINARY_DIR}/src/common_db)
include_directories(${SYD_BINARY_DIR}/src/core)
include_directories(${SYD_BINARY_DIR}/src/std_db)
include_directories(${SYD_BINARY_DIR}/src/ext)
# ----------------------------------------------------------
