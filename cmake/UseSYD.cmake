# This file sets up include directories, link directories, and
# compiler settings for a project to use SYD.  It should not be
# included directly, but rather through the SYD_USE_FILE setting
# obtained from SYDConfig.cmake.

#----------------------------------------------------------
# Find ITK (required)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

#----------------------------------------------------------
# Add include directories needed to use SYD.
include_directories(BEFORE ${SYD_INCLUDE_DIRS})

#----------------------------------------------------------
# Add link directories needed to use SYD.
link_directories(${SYD_LIBRARY_DIRS})

#----------------------------------------------------------
# Helping macro
macro(DD in)
    message(${in}=${${in}})
endmacro(DD)
#----------------------------------------------------------


#----------------------------------------------------------
# To compile odb files // FIXME use include (?)
macro(WRAP_ODB ODB_SRCS)
  foreach(ODB_FILES ${ARGN})
    get_filename_component(ODB_BASEFILENAME ${ODB_FILES} NAME_WE)
    get_filename_component(ODB_FILES_ABS ${ODB_FILES} ABSOLUTE)
    DD(SCHEMA_NAME)
    DD(ODB_FILES_ABS)
    DD(ODB_BASEFILENAME)
    set(ODB_CXX ${ODB_BASEFILENAME}-odb.cxx)
    set(ODB_IXX ${ODB_BASEFILENAME}-odb.ixx)
    set(ODB_HXX ${ODB_BASEFILENAME}-odb.hxx)
    set(ODB_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ODB_CXX} ${CMAKE_CURRENT_BINARY_DIR}/${ODB_IXX} ${CMAKE_CURRENT_BINARY_DIR}/${ODB_HXX})
    if(EXISTS ${ODB_FILES_ABS})
      add_custom_command(OUTPUT ${ODB_OUTPUT}
        COMMAND odb
        ARGS --std c++11 --database sqlite -I${SYD_SOURCE_DIR}/src/std_db -I${SYD_SOURCE_DIR}/src/core --generate-schema --schema-format embedded --generate-query --sqlite-override-null --schema-name ${SCHEMA_NAME} ${ODB_FILES_ABS}
        DEPENDS ${ODB_FILES_ABS})
    else()
      message(FATAL_ERROR "Error odb cannot file the following file: " ${ODB_FILES_ABS})
    endif()
    set(${ODB_SRCS} ${${ODB_SRCS}} ${ODB_OUTPUT})
    DD(ODB_OUTPUT)
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
# Need thoses directories to include odb generated files
include_directories(${SYD_BUILD_DIR}/src/)
include_directories(${SYD_BUILD_DIR}/src/std_db)


# ----------------------------------------------------------
# Attempt to find gengetopt. If not found, compile it.
find_program(GENGETOPT gengetopt)
if(GENGETOPT STREQUAL "GENGETOPT-NOTFOUND")
  get_filename_component(SYD_CMAKE_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
  add_subdirectory(${SYD_CMAKE_DIR}/../utilities/gengetopt ${CMAKE_CURRENT_BINARY_DIR}/gengetopt)
else(GENGETOPT STREQUAL "GENGETOPT-NOTFOUND")
  add_executable(gengetopt IMPORTED)
  set_property(TARGET gengetopt PROPERTY IMPORTED_LOCATION ${GENGETOPT})
endif(GENGETOPT STREQUAL "GENGETOPT-NOTFOUND")

macro(WRAP_GGO GGO_SRCS)
  foreach(GGO_FILE ${ARGN})
    get_filename_component(GGO_BASEFILENAME ${GGO_FILE} NAME_WE)
    get_filename_component(GGO_FILE_ABS ${GGO_FILE} ABSOLUTE)
    set(GGO_H ${GGO_BASEFILENAME}_ggo.h)
    set(GGO_C ${GGO_BASEFILENAME}_ggo.c)
    set(GGO_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${GGO_H} ${CMAKE_CURRENT_BINARY_DIR}/${GGO_C})
    add_custom_command(OUTPUT ${GGO_OUTPUT}
                       COMMAND gengetopt
                       ARGS < ${GGO_FILE_ABS}
                              --output-dir=${CMAKE_CURRENT_BINARY_DIR}
                              --arg-struct-name=args_info_${GGO_BASEFILENAME}
                              --func-name=cmdline_parser_${GGO_BASEFILENAME}
                              --file-name=${GGO_BASEFILENAME}_ggo
                              --unamed-opts
                              --conf-parser
                              --include-getopt
                       DEPENDS ${GGO_FILE_ABS}
                      )
    set(${GGO_SRCS} ${${GGO_SRCS}} ${GGO_OUTPUT})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
  endforeach(GGO_FILE)
  set_source_files_properties(${${GGO_SRCS}} PROPERTIES GENERATED TRUE)
  if(CMAKE_COMPILER_IS_GNUCXX)
    find_program(DEFAULT_GCC gcc)
    exec_program(${DEFAULT_GCC} ARGS "-dumpversion" OUTPUT_VARIABLE GCCVER)
    if("${GCCVER}" VERSION_GREATER "4.5.2")
      set_source_files_properties(${${GGO_SRCS}} PROPERTIES COMPILE_FLAGS "-Wno-unused-but-set-variable")
    endif("${GCCVER}" VERSION_GREATER "4.5.2")
  endif(CMAKE_COMPILER_IS_GNUCXX)
endmacro(WRAP_GGO)
