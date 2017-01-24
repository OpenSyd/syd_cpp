

# only depends on sydCore and odb
# no dependence on sydFit and sydItkUtils

#----------------------------------------------------------
# First create the common database schema
set(SCHEMA_NAME sydCommonDatabase)
wrap_odb(sydCore sydDatabaseInformation.h)
wrap_odb(sydCore sydDatabaseSchemaName.h)
wrap_odb(sydCore sydRecord.h)
wrap_odb(sydCore sydRecordHistory.h)
wrap_odb(sydCore sydRecordWithHistory.h)
wrap_odb(sydCore sydFile.h)
wrap_odb(sydCore sydTag.h)
wrap_odb(sydCore sydRecordWithTags.h)
wrap_odb(sydCore sydRecordWithComments.h)
wrap_odb(sydCore sydRecordWithMD5Signature.h)

set(sydCommonDatabase_src
  sydVersion.h
  sydPrintTable.cxx

  # sydDatabaseDescription.cxx
  # sydTableDescription.cxx
  # sydFieldDescription.cxx

  sydDatabaseSchemaName-odb.cxx
  sydDatabaseSchemaName-schema.cxx
  sydDatabaseInformation-odb.cxx
  sydDatabaseInformation-schema.cxx

  sydRecord-odb.cxx
  sydRecord-schema.cxx
  sydRecord.cxx
  sydRecordTraitsBase.cxx

  sydFieldBase.cxx
  sydFieldType.cxx

  ## must be before the next files !
  sydTagTraits.cxx
  sydFileTraits.cxx

  sydRecordHistory-odb.cxx
  sydRecordHistory-schema.cxx
  sydRecordHistory.cxx
  sydRecordWithHistory-odb.cxx
  sydRecordWithHistory-schema.cxx
  sydRecordWithHistory.cxx
  sydRecordWithMD5Signature-odb.cxx
  sydRecordWithMD5Signature-schema.cxx
  sydRecordWithMD5Signature.cxx
  sydFile-odb.cxx
  sydFile-schema.cxx
  sydFile.cxx
  sydTag-odb.cxx
  sydTag-schema.cxx
  sydTag.cxx
  sydRecordWithTags-odb.cxx
  sydRecordWithTags-schema.cxx
  sydRecordWithTags.cxx
  sydRecordWithComments-odb.cxx
  sydRecordWithComments-schema.cxx
  sydRecordWithComments.cxx

  sydDatabase.cxx
  sydDatabaseManager.cxx
  sydDatabaseInformation.cxx
  sydPluginManager.cxx
  sydDatabaseInformation.cxx

  sydCommonDatabase.cxx

  sydRecordHelper.cxx
  sydTagHelper.cxx
  sydFileHelper.cxx
  )

add_library(sydCommonDatabaseObjects OBJECT ${sydCommonDatabase_src})
add_library(sydCommonDatabase SHARED $<TARGET_OBJECTS:sydCommonDatabaseObjects>)
if (APPLE)
  target_link_libraries(sydCommonDatabase ${SYD_CORE_LIBRARIES})
endif(APPLE)

#----------------------------------------------------------
wrap_ggo(sydCreateDatabase_GGO_C sydCreateDatabase.ggo sydCommandLineDefaultWithoutDB.ggo)
add_executable(sydCreateDatabase sydCreateDatabase.cxx ${sydCreateDatabase_GGO_C})
target_link_libraries(sydCreateDatabase ${SYD_COMMON_DB_LIBRARY})
#----------------------------------------------------------

#----------------------------------------------------------
wrap_ggo(sydInsert_GGO_C sydInsert.ggo sydCommandLineDefault.ggo)
add_executable(sydInsert sydInsert.cxx ${sydInsert_GGO_C})
target_link_libraries(sydInsert ${SYD_COMMON_DB_LIBRARY})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydFind_GGO_C sydFind.ggo sydCommandLineDefault.ggo)
add_executable(sydFind sydFind.cxx ${sydFind_GGO_C})
target_link_libraries(sydFind ${SYD_COMMON_DB_LIBRARY})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydDelete_GGO_C sydDelete.ggo sydCommandLineDefault.ggo)
add_executable(sydDelete sydDelete.cxx ${sydDelete_GGO_C})
target_link_libraries(sydDelete ${SYD_COMMON_DB_LIBRARY})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydCheck_GGO_C sydCheck.ggo sydCommandLineDefault.ggo)
add_executable(sydCheck sydCheck.cxx ${sydCheck_GGO_C})
target_link_libraries(sydCheck ${SYD_COMMON_DB_LIBRARY})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydUpdate_GGO_C sydUpdate.ggo sydCommandLineDefault.ggo)
add_executable(sydUpdate sydUpdate.cxx ${sydUpdate_GGO_C})
target_link_libraries(sydUpdate ${SYD_COMMON_DB_LIBRARY})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydInfo_GGO_C sydInfo.ggo sydCommandLineDefault.ggo)
add_executable(sydInfo sydInfo.cxx ${sydInfo_GGO_C})
target_link_libraries(sydInfo ${SYD_COMMON_DB_LIBRARY})
#----------------------------------------------------------
