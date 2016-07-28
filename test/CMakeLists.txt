

#----------------------------------------------------------
# Needed for sydPatient.h for example
include_directories(${SYD_INCLUDE_DIRS})

# Needed for odb generated sydDatabaseInformation-odb.hxx
include_directories(${SYD_BINARY_DIR}/src/core)

# Test1 with plugin
add_executable(syd_test1 syd_test1.cxx)
target_link_libraries(syd_test1 sydCommonDatabase)
add_test(NAME syd_test1
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test1
  WORKING_DIRECTORY ${SYD_BINARY_DIR}/test)

# Test2 with StandardDatabase
add_executable(syd_test2 syd_test2.cxx)
target_link_libraries(syd_test2 StandardDatabase)
add_test(NAME syd_test2
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/syd_test2
  WORKING_DIRECTORY ${SYD_BINARY_DIR}/test)
