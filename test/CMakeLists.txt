


include_directories(${PROJECT_SOURCE_DIR}/src/core)
include_directories(${PROJECT_BINARY_DIR}/src/std_db)
include_directories(${PROJECT_SOURCE_DIR}/src/std_db)


# test create database
add_executable(test_sydCreateDatabase test_sydCreateDatabase.cxx)
target_link_libraries(test_sydCreateDatabase sydStudyDatabase ${SYD_LIBS})
add_test(NAME test_sydCreateDatabase
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_sydCreateDatabase
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)
