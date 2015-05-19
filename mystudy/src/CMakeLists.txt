

#DD(SYD_SOURCE_DIR)

#include_directories(${AA}/src/core/)
#DD(BB)
DD(SYD_BUILD_DIR)

# Need this two directories to find compiled odb files
# include_directories(${SYD_BUILD_DIR}/src/)

# include_directories(${SYD_BUILD_DIR}/src/core)
# include_directories(${SYD_BUILD_DIR}/src/std_db)

# get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
# message("inc_dirs = ${inc_dirs}")

#include_directories(${BB}/src/std_db/)
#include(${SYD_USE_FILE})

# include_directories(${SYD_SOURCE_DIR}/src/)
#include_directories(${SYD_SOURCE_DIR}/src/std_db)
#include_directories(${SYD_SOURCE_DIR}/src/core)

#include_directories(${SYD_INCLUDE_DIRS})

#add_subdirectory(${SYD_BUILD_DIR} src/std_db)

#----------------------------------------------------------
# mystudy database : MyStudyDatabase
set(SCHEMA_NAME extendedStandardSchema)
wrap_odb(MyStudyDatabase extPatient.h)
#wrap_odb(MyStudyDatabase a.h)
# wrap_odb(MyStudyDatabase sydInjection.h)
# wrap_odb(MyStudyDatabase sydDicomSerie.h)
# wrap_odb(MyStudyDatabase sydDicomFile.h)
# wrap_odb(MyStudyDatabase sydTimepoint.h)
# wrap_odb(MyStudyDatabase sydTag.h)


add_library(MyStudyDatabase SHARED
  extendedDatabase.cxx
  extendedDatabasePlugin.cxx
  extPatient-odb.cxx
  # sydFile-odb.cxx sydFile.cxx
  # sydTag-odb.cxx sydTag.cxx
  # sydInjection-odb.cxx sydInjection.cxx
  # sydDicomSerie-odb.cxx sydDicomSerie.cxx
  # sydDicomFile-odb.cxx sydDicomFile.cxx
  # sydTimepoint-odb.cxx sydTimepoint.cxx
  )
## to declare dependencies
target_link_libraries(MyStudyDatabase ${SYD_LIBRARIES})


#----------------------------------------------------------
# Specific tool for extTestTool
wrap_ggo(extTestTool extTestTool.ggo)
add_executable(extTestTool extTestTool.cxx extTestTool_ggo.c)
target_link_libraries(extTestTool MyStudyDatabase)
#----------------------------------------------------------
