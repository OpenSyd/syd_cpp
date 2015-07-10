

## Both needed
include_directories(${SYD_BINARY_DIR}/src/core)
include_directories(${SYD_SOURCE_DIR}/src/std_db)

## test extend db
set(SCHEMA_NAME ExtendedDatabase)
wrap_odb(extExtendedDatabase extPatient.h)

add_library(extExtendedDatabase SHARED
  extExtendedDatabase.cxx
  extExtendedDatabasePlugin.cxx
  extPatient-odb.cxx extPatient.cxx
  )

target_link_libraries(extExtendedDatabase ${SYD_LIBRARIES})

# Not really a tool, for debug only
# wrap_ggo(sydTest sydTest.ggo)
# add_executable(sydTest sydTest.cxx sydTest_ggo.c)
# target_link_libraries(sydTest extExtendedDatabase)



# #----------------------------------------------------------
# # Define the schema of MyDatabase
# set(SCHEMA_NAME SynfrizzDatabase)
# # wrap_odb(SynfrizzDatabase sfzActivity.h)
# #wrap_odb(SynfrizzDatabase sydPatient.h)
# wrap_odb(SynfrizzDatabase sfzPatient.h)
# #wrap_odb(sfzSynfrizzDatabase sfzSynfrizzDatabaseSchema.h)
# add_library(SynfrizzDatabase SHARED
#   sfzSynfrizzDatabase.cxx
#   sfzSynfrizzDatabasePlugin.cxx
#   # sfzSynfrizzDatabaseSchema-odb.cxx
#   sfzPatient-odb.cxx
#   sfzPatient.cxx
#   # sydPatient-odb.cxx
#   # sfzActivity-odb.cxx
#   # sfzActivity.cxx
# ###  sfzPatientTable.cxx

#   )
# target_link_libraries(SynfrizzDatabase sydStandardDatabase ${SYD_CORE_LIBRARIES})
