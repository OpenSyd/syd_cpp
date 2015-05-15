
include_directories(${PROJECT_SOURCE_DIR}/src/core)
include_directories(${PROJECT_BINARY_DIR}/src/std_db)
include_directories(${PROJECT_SOURCE_DIR}/src/std_db)
# include_directories(../src/std_db)

#----------------------------------------------------------
# test create database
add_executable(test_sydCreateDatabase test_sydCreateDatabase.cxx)
target_link_libraries(test_sydCreateDatabase sydStandardDatabase ${SYD_LIBS})
add_test(NAME test_sydCreateDatabase
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_sydCreateDatabase
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)


add_executable(test_tables test_tables.cxx)
target_link_libraries(test_tables sydStandardDatabase ${SYD_LIBS})
add_test(NAME test_tables
  COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_tables
  WORKING_DIRECTORY ${SYD_SOURCE_DIR}/test)


# #----------------------------------------------------------
# # Policy for shared library on OSX
# cmake_policy(SET CMP0042 NEW) # CMake 3.0
# #----------------------------------------------------------


# #----------------------------------------------------------
# # Extend standard database
# set(SCHEMA_NAME extendedSchema)
# wrap_odb(extendedDatabase extPatient.h)
# add_library(extendedDatabase SHARED
#   extendedDatabase.cxx
#   extendedDatabasePlugin.cxx
#   extPatient-odb.cxx)
# ## to declare dependencies
# target_link_libraries(extendedDatabase sydStandardDatabase ${SYD_LIBS})

# #----------------------------------------------------------
# # test extended database
# add_executable(test_extendedDB test_extendedDB.cxx)
# target_link_libraries(test_extendedDB extendedDatabase ${SYD_LIBS})
# add_test(NAME test_extendedDB
#   COMMAND ${EXECUTABLE_OUTPUT_PATH}/test_extendedDB
#   WORKING_DIRECTORY ${SYD_SOURCE_DIR}/src/test)
