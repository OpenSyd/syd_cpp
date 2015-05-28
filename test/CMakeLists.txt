

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


#----------------------------------------------------------
# test create timepoint
add_executable(test_timepoint_builder test_timepoint_builder.cxx)
target_link_libraries(test_timepoint_builder sydStandardDatabase)
add_test(NAME test_timepoint_builder
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_timepoint_builder
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)


#----------------------------------------------------------
# test image builder
add_executable(test_image_builder test_image_builder.cxx)
target_link_libraries(test_image_builder sydStandardDatabase)
add_test(NAME test_image_builder
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_image_builder
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)
