
#----------------------------------------------------------
# First create the common database schema
set(SCHEMA_NAME sydCommonSchema)
wrap_odb(sydCore sydDatabaseInformation.h)
wrap_odb(sydCore sydDatabaseSchemaName.h)

add_library(sydCommonSchema SHARED
  sydDatabaseSchemaName-odb.cxx
  sydDatabaseInformation-odb.cxx
  sydDatabaseInformation.cxx)

# needed for clang osx
target_link_libraries(sydCommonSchema
  ${DCMTK_LIBRARIES}
  odb-sqlite odb sqlite3 z
  ${ITK_LIBRARIES})

#----------------------------------------------------------
# Common library
add_library(sydCore
  md5.cpp
  sydCommon.cxx
  sydLog.cxx
  sydDatabaseInformation.cxx
  sydPrintTable.cxx
  sydImage.cxx
  sydPluginManager.cxx
  sydException.cxx
  sydDatabase.cxx
  sydDatabaseManager.cxx
  sydDicomCommon.cxx
  sydTableElement.cxx
)

target_link_libraries(sydCore
  ${DCMTK_LIBRARIES}
  sydCommonSchema
  odb-sqlite odb sqlite3 z
  ${ITK_LIBRARIES})

#----------------------------------------------------------
