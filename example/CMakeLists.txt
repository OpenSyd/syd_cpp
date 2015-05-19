# ----------------------------------------------------------
# MyDatabase
# ----------------------------------------------------------
project(mydatabase)

#----------------------------------------------------------
cmake_minimum_required(VERSION 2.8)
# Policy for shared library on OSX
cmake_policy(SET CMP0042 NEW)
#----------------------------------------------------------


#----------------------------------------------------------
find_package(SYD REQUIRED)
include(${SYD_USE_FILE})

# The following variable are available from SYD_USE_FILE:
# DD(ITK_USE_FILE)
# DD(ITK_LIBRARIES)
# DD(SYD_LIBRARIES)
# DD(SYD_INCLUDE_DIRS)
# DD(SYD_LIBRARY_DIRS)
# DD(SYD_SOURCE_DIR)

#----------------------------------------------------------
# Define the schema of MyDatabase
set(SCHEMA_NAME MySchema)
wrap_odb(MyDatabase extPatient.h)

add_library(MyDatabase SHARED
  extMyDatabase.cxx
  extMyDatabasePlugin.cxx
  extPatient-odb.cxx extPatient.cxx
  )
## to declare dependencies
target_link_libraries(MyDatabase ${SYD_LIBRARIES})

#----------------------------------------------------------
# Tool that use MyDatabase
wrap_ggo(extTestTool extTestTool.ggo)
add_executable(extTestTool extTestTool.cxx extTestTool_ggo.c)
target_link_libraries(extTestTool MyDatabase)
#----------------------------------------------------------
