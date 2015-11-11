

# only depends on sydCore and odb
# no dependence on sydFit and sydItkUtils

#----------------------------------------------------------
# First create the common database schema
set(SCHEMA_NAME sydCommonDatabase)
wrap_odb(sydCore sydDatabaseInformation.h)
wrap_odb(sydCore sydDatabaseSchemaName.h)
wrap_odb(sydCore sydRecord.h)

set(sydCommonDatabase_src
  sydDatabaseSchemaName-odb.cxx
  sydDatabaseInformation-odb.cxx
  sydRecord-odb.cxx sydRecord.cxx
  sydDatabase.cxx
  sydDatabaseManager.cxx
  sydDatabaseInformation.cxx
  sydPluginManager.cxx
  sydDatabaseInformation.cxx)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # with clang on osx (apple), needed for static member def
  message(STATUS "Clang compiler found (special case for static external in shared library)")
  add_library(sydCommonDatabase SHARED ${sydCommonDatabase_src} sydStatic.cxx )
else()
  # shared is needed here
  add_library(sydCommonDatabase SHARED ${sydCommonDatabase_src})
endif()

target_link_libraries(sydCommonDatabase
  sydCore odb-sqlite odb sqlite3 z)

#----------------------------------------------------------
wrap_ggo(sydCreateDatabase_GGO_C sydCreateDatabase.ggo sydCommandLineDefaultWithoutDB.ggo)
add_executable(sydCreateDatabase sydCreateDatabase.cxx ${sydCreateDatabase_GGO_C})
target_link_libraries(sydCreateDatabase sydCommonDatabase)
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydInsert_GGO_C sydInsert.ggo sydCommandLineDefault.ggo)
add_executable(sydInsert sydInsert.cxx ${sydInsert_GGO_C})
target_link_libraries(sydInsert sydCommonDatabase)
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydFind_GGO_C sydFind.ggo sydCommandLineDefault.ggo)
add_executable(sydFind sydFind.cxx ${sydFind_GGO_C})
target_link_libraries(sydFind sydCommonDatabase)
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydDelete_GGO_C sydDelete.ggo sydCommandLineDefault.ggo)
add_executable(sydDelete sydDelete.cxx ${sydDelete_GGO_C})
target_link_libraries(sydDelete sydCommonDatabase)
#----------------------------------------------------------
