

## Needed to find sydRecord-odb.hxx
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

target_link_libraries(extExtendedDatabase sydStandardDatabase  ${SYD_CORE_LIBRARIES})

# Not really a tool, for debug only
wrap_ggo(sydTest sydTest.ggo)
add_executable(sydTest sydTest.cxx sydTest_ggo.c)
target_link_libraries(sydTest extExtendedDatabase)
