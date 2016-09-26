

#----------------------------------------------------------
# Needed for sydPatient.h for example
include_directories(${SYD_INCLUDE_DIRS})

# Needed for odb generated sydDatabaseInformation-odb.hxx
include_directories(${SYD_BINARY_DIR}/src/core)

# common functions
# add_library(sydTestLib sydTestHelper.cxx)
# target_link_libraries(sydTestLib sydCommonDatabase sydStandardDatabase)

# Test1 with plugin
add_executable(syd_test1 syd_test1.cxx)
target_link_libraries(syd_test1 ${SYD_COMMON_DB_LIBRARY})
add_test(NAME syd_test1
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test1
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

# Test2 with sydStandardDatabase
add_executable(syd_test2 syd_test2.cxx)
target_link_libraries(syd_test2 ${SYD_STD_DB_LIBRARY})
add_test(NAME syd_test2
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test2
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

# Test3 dicom
wrap_ggo(syd_test3_GGO_C syd_test3.ggo ../src/common_db/sydCommandLineDefault.ggo)
add_executable(syd_test3 syd_test3.cxx ${syd_test3_GGO_C})
target_link_libraries(syd_test3 ${SYD_STD_DB_LIBRARY})
add_test(NAME syd_test3
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test3)

# Test4 image
wrap_ggo(syd_test4_GGO_C syd_test4.ggo ../src/common_db/sydCommandLineDefault.ggo)
add_executable(syd_test4 syd_test4.cxx ${syd_test4_GGO_C})
target_link_libraries(syd_test4 ${SYD_STD_DB_LIBRARY})
add_test(NAME syd_test4
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test4)

# Test5 roi
wrap_ggo(syd_test5_GGO_C syd_test5.ggo ../src/common_db/sydCommandLineDefault.ggo)
add_executable(syd_test5 syd_test5.cxx ${syd_test5_GGO_C})
target_link_libraries(syd_test5 ${SYD_STD_DB_LIBRARY})
add_test(NAME syd_test5
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test5)


# Test6 roi
wrap_ggo(syd_test6_GGO_C syd_test6.ggo ../src/common_db/sydCommandLineDefault.ggo)
add_executable(syd_test6 syd_test6.cxx ${syd_test6_GGO_C})
target_link_libraries(syd_test6 ${SYD_STD_DB_LIBRARY})
add_test(NAME syd_test6
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test6)
