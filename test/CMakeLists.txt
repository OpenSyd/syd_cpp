

#----------------------------------------------------------
include_directories(${SYD_INCLUDE_DIRS})

#----------------------------------------------------------
# test create database
add_executable(test_create_database test_create_database.cxx)
target_link_libraries(test_create_database sydStandardDatabase)
add_test(NAME test_create_database
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_create_database
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

#----------------------------------------------------------
# test create some tables
add_executable(test_tables test_tables.cxx)
target_link_libraries(test_tables sydStandardDatabase)
add_test(NAME test_tables
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_tables
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)
