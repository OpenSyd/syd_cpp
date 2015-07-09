

#----------------------------------------------------------
# Needed for sydPatient.h for example
include_directories(${SYD_INCLUDE_DIRS})

# Needed for odb generated sydDatabaseInformation-odb.hxx
include_directories(${SYD_BINARY_DIR}/src/core)
include_directories(${SYD_BINARY_DIR}/src/ext)

# Test creation of database
add_executable(test_create_database test_create_database.cxx)
target_link_libraries(test_create_database extExtendedDatabase)
add_test(NAME test_create_database
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_create_database
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

# Test insert
add_executable(test_insert test_insert.cxx)
target_link_libraries(test_insert extExtendedDatabase)
add_test(NAME test_insert
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_insert
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

# Test update
add_executable(test_update test_update.cxx)
target_link_libraries(test_update extExtendedDatabase)
add_test(NAME test_update
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_update
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

# Test query
add_executable(test_query test_query.cxx)
target_link_libraries(test_query extExtendedDatabase)
add_test(NAME test_query
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_query
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

# Test dump
add_executable(test_dump test_dump.cxx)
target_link_libraries(test_dump extExtendedDatabase)
add_test(NAME test_dump
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_dump
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

# Test find
add_executable(test_find test_find.cxx)
target_link_libraries(test_find extExtendedDatabase)
add_test(NAME test_find
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_find
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)


# Test delete
add_executable(test_delete test_delete.cxx)
target_link_libraries(test_delete extExtendedDatabase)
add_test(NAME test_delete
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_delete
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

# Test  dynamic_fields
add_executable(test_dynamic_fields test_dynamic_fields.cxx)
target_link_libraries(test_dynamic_fields extExtendedDatabase)
add_test(NAME test_dynamic_fields
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_dynamic_fields
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)


# #----------------------------------------------------------
# # test create database
# add_executable(test_create_database test_create_database.cxx)
# target_link_libraries(test_create_database sydStandardDatabase)
# add_test(NAME test_create_database
#   COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_create_database
#   WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)

# #----------------------------------------------------------
# # test create some tables
# add_executable(test_tables test_tables.cxx)
# target_link_libraries(test_tables sydStandardDatabase)
# add_test(NAME test_tables
#   COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_tables
#   WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)


# #----------------------------------------------------------
# # test create timepoint
# # add_executable(test_timepoint_builder test_timepoint_builder.cxx)
# # target_link_libraries(test_timepoint_builder sydStandardDatabase)
# # add_test(NAME test_timepoint_builder
# #   COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_timepoint_builder
# #   WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)


# #----------------------------------------------------------
# # test image builder
# add_executable(test_image_builder test_image_builder.cxx)
# target_link_libraries(test_image_builder sydStandardDatabase)
# add_test(NAME test_image_builder
#   COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_image_builder
#   WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)
