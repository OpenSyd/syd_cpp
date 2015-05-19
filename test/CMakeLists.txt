

#----------------------------------------------------------
include_directories(${SYD_INCLUDE_DIRS})

#----------------------------------------------------------
# test create database
add_executable(test_sydCreateDatabase test_sydCreateDatabase.cxx)
target_link_libraries(test_sydCreateDatabase sydStandardDatabase)
add_test(NAME test_sydCreateDatabase
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_sydCreateDatabase
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

#----------------------------------------------------------
# test create some tables
add_executable(test_tables test_tables.cxx)
target_link_libraries(test_tables sydStandardDatabase)
add_test(NAME test_tables
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_tables
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)
