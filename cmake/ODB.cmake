
#----------------------------------------------------------
macro(WRAP_ODB ODB_SRCS)
  foreach(ODB_FILES ${ARGN})
    get_filename_component(ODB_BASEFILENAME ${ODB_FILES} NAME_WE)
    get_filename_component(ODB_FILES_ABS ${ODB_FILES} ABSOLUTE)
    set(ODB_CXX ${ODB_BASEFILENAME}-odb.cxx)
    set(ODB_IXX ${ODB_BASEFILENAME}-odb.ixx)
    set(ODB_HXX ${ODB_BASEFILENAME}-odb.hxx)
    set(ODB_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ODB_CXX} ${CMAKE_CURRENT_BINARY_DIR}/${ODB_IXX} ${CMAKE_CURRENT_BINARY_DIR}/${ODB_HXX})
    add_custom_command(OUTPUT ${ODB_OUTPUT}
      COMMAND odb
      ARGS --std c++11 --database sqlite --generate-query ${ODB_FILES_ABS}
      DEPENDS ${ODB_FILES_ABS}
      )
    set(${ODB_SRCS} ${${ODB_SRCS}} ${ODB_OUTPUT})
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  endforeach(ODB_FILES)
  set_source_files_properties(${${ODB_SRCS}} PROPERTIES GENERATED TRUE)
endmacro(WRAP_ODB)
#----------------------------------------------------------
