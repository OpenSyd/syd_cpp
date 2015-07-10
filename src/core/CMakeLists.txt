
#----------------------------------------------------------
# First create the common database schema
set(SCHEMA_NAME sydCommonSchema)
wrap_odb(sydCore sydDatabaseInformation.h)
wrap_odb(sydCore sydDatabaseSchemaName.h)
wrap_odb(sydCore sydRecord.h)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  message(STATUS "Clang compiler found (special case for static external in shared library)")
  add_library(sydCommonSchema
    sydDatabaseSchemaName-odb.cxx
    sydDatabaseInformation-odb.cxx
    sydRecord-odb.cxx sydRecord.cxx
    sydLog.cxx
    sydStatic.cxx # with clang on osx (apple), needed for static member def
    sydPrintTable.cxx # needed in sydRecord
    sydDatabaseInformation.cxx)
else()
  add_library(sydCommonSchema
    sydDatabaseSchemaName-odb.cxx
    sydDatabaseInformation-odb.cxx
    sydRecord-odb.cxx sydRecord.cxx
    sydLog.cxx
    sydPrintTable.cxx # needed in sydRecord
    sydDatabaseInformation.cxx)
endif()

target_link_libraries(sydCommonSchema
  ${DCMTK_LIBRARIES}
  odb-sqlite odb sqlite3 z
  ${ITK_LIBRARIES})

#----------------------------------------------------------
# Common library
include_directories(${SYD_SOURCE_DIR}/src/pstreams)
add_library(sydCore
  md5.cpp
  sydCommon.cxx
  sydLog.cxx
  sydDatabaseInformation.cxx
  sydImageUtils.cxx
  sydPluginManager.cxx
  sydException.cxx
  sydDatabase.cxx
#  sydTestUtils.cxx
  sydDatabaseManager.cxx
  sydDicomCommon.cxx
#  sydTableElementBase.cxx
)

target_link_libraries(sydCore
  ${DCMTK_LIBRARIES}
  sydCommonSchema
  odb-sqlite odb sqlite3 z
  ${ITK_LIBRARIES})

#----------------------------------------------------------
