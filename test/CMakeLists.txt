

#----------------------------------------------------------
# Needed for sydPatient.h for example
include_directories(${SYD_INCLUDE_DIRS})

# Needed for odb generated sydDatabaseInformation-odb.hxx
include_directories(${SYD_BINARY_DIR}/src/core)

# common functions
add_library(sydTestLib sydTestHelper.cxx)
target_link_libraries(sydTestLib sydCommonDatabase sydStandardDatabase)

# Test1 with plugin
add_executable(syd_test1 syd_test1.cxx)
target_link_libraries(syd_test1 sydCommonDatabase)
add_test(NAME syd_test1
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test1
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

# Test2 with sydStandardDatabase
add_executable(syd_test2 syd_test2.cxx)
target_link_libraries(syd_test2 sydStandardDatabase)
add_test(NAME syd_test2
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test2
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

# Test3 dicom
wrap_ggo(syd_test3_GGO_C syd_test3.ggo ../src/common_db/sydCommandLineDefault.ggo)
add_executable(syd_test3 syd_test3.cxx ${syd_test3_GGO_C})
target_link_libraries(syd_test3 sydStandardDatabase)
add_test(NAME syd_test3
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test3)

# Test4 image
wrap_ggo(syd_test4_GGO_C syd_test4.ggo ../src/common_db/sydCommandLineDefault.ggo)
add_executable(syd_test4 syd_test4.cxx ${syd_test4_GGO_C})
target_link_libraries(syd_test4 sydCommonDatabase sydStandardDatabase sydTestLib)
add_test(NAME syd_test4
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test4)

# Test5 roi
wrap_ggo(syd_test5_GGO_C syd_test5.ggo ../src/common_db/sydCommandLineDefault.ggo)
add_executable(syd_test5 syd_test5.cxx ${syd_test5_GGO_C})
target_link_libraries(syd_test5 sydCommonDatabase sydStandardDatabase sydTestLib)
add_test(NAME syd_test5
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test5)
