

## Both needed
include_directories(${SYD_BINARY_DIR}/src/core)
include_directories(${SYD_SOURCE_DIR}/src/std_db)

## Following lines needed if outside syd project
# set(SYD_DIR ${SYD_BINARY_DIR})
# find_package(SYD REQUIRED)
# include(${SYD_USE_FILE})

## test extend db
set(SCHEMA_NAME ExtendedDatabase)
wrap_odb(extExtendedDatabase extPatient.h)

add_library(extExtendedDatabase SHARED
  extExtendedDatabase.cxx
  extExtendedDatabasePlugin.cxx
  extPatient-odb.cxx
  extPatient-schema.cxx
  extPatient.cxx
  )

target_link_libraries(extExtendedDatabase ${SYD_LIBRARIES})
