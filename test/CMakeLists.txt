

#----------------------------------------------------------
# Needed for sydPatient.h for example
include_directories(${SYD_INCLUDE_DIRS})

# Needed for odb generated sydDatabaseInformation-odb.hxx
include_directories(${SYD_BINARY_DIR}/src/core)

# common functions
add_library(sydTestLib sydTestHelper.cxx)

# Test1 with plugin
add_executable(syd_test1 syd_test1.cxx)
target_link_libraries(syd_test1 sydCommonDatabase)
add_test(NAME syd_test1
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test1
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

# Test2 with StandardDatabase
add_executable(syd_test2 syd_test2.cxx)
target_link_libraries(syd_test2 StandardDatabase)
add_test(NAME syd_test2
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test2
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

# Test3 dicom
wrap_ggo(syd_test3_GGO_C syd_test3.ggo ../src/common_db/sydCommandLineDefault.ggo)
add_executable(syd_test3 syd_test3.cxx ${syd_test3_GGO_C})
target_link_libraries(syd_test3 StandardDatabase)
add_test(NAME syd_test3
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test3)

# Test4 dicom
wrap_ggo(syd_test4_GGO_C syd_test4.ggo ../src/common_db/sydCommandLineDefault.ggo)
add_executable(syd_test4 syd_test4.cxx ${syd_test4_GGO_C})
target_link_libraries(syd_test4 StandardDatabase sydTestLib)
add_test(NAME syd_test4
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test4)
