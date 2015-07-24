


## Needed to find sydRecord-odb.hxx
include_directories(${SYD_BINARY_DIR}/src/core)


#----------------------------------------------------------
wrap_ggo(sydCreateDatabase_GGO_C sydCreateDatabase.ggo sydCommandLineDefaultWithoutDB.ggo)
add_executable(sydCreateDatabase sydCreateDatabase.cxx ${sydCreateDatabase_GGO_C})
target_link_libraries(sydCreateDatabase ${SYD_CORE_LIBRARIES})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydInsert_GGO_C sydInsert.ggo sydCommandLineDefault.ggo)
add_executable(sydInsert sydInsert.cxx ${sydInsert_GGO_C})
target_link_libraries(sydInsert ${SYD_CORE_LIBRARIES})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydDump_GGO_C sydDump.ggo sydCommandLineDefault.ggo)
add_executable(sydDump sydDump.cxx ${sydDump_GGO_C})
target_link_libraries(sydDump ${SYD_CORE_LIBRARIES})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydFind_GGO_C sydFind.ggo sydCommandLineDefault.ggo)
add_executable(sydFind sydFind.cxx ${sydFind_GGO_C})
target_link_libraries(sydFind ${SYD_CORE_LIBRARIES})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydDelete_GGO_C sydDelete.ggo sydCommandLineDefault.ggo)
add_executable(sydDelete sydDelete.cxx ${sydDelete_GGO_C})
target_link_libraries(sydDelete ${SYD_CORE_LIBRARIES})
#----------------------------------------------------------
