
#----------------------------------------------------------
# Standard database : StandardDatabase
set(SCHEMA_NAME StandardSchema)
wrap_odb(sydStandardDatabase sydPatient.h)
wrap_odb(sydStandardDatabase sydFile.h)
wrap_odb(sydStandardDatabase sydInjection.h)
wrap_odb(sydStandardDatabase sydDicomSerie.h)
wrap_odb(sydStandardDatabase sydDicomFile.h)
wrap_odb(sydStandardDatabase sydTimepoint.h)
wrap_odb(sydStandardDatabase sydTag.h)

add_library(sydStandardDatabase SHARED
  sydStandardDatabase.cxx
  sydStandardDatabasePlugin.cxx
  sydPatient-odb.cxx sydPatient.cxx
  sydFile-odb.cxx sydFile.cxx
  sydTag-odb.cxx sydTag.cxx
  sydInjection-odb.cxx sydInjection.cxx
  sydDicomSerie-odb.cxx sydDicomSerie.cxx
  sydDicomFile-odb.cxx sydDicomFile.cxx
  sydTimepoint-odb.cxx sydTimepoint.cxx
  sydDicomSerieBuilder.cxx
  sydTimepointBuilder.cxx
  )
## to declare dependencies
target_link_libraries(sydStandardDatabase ${SYD_LIBS})

# Specific tool for DicomSerie
wrap_ggo(sydInsertDicom sydInsertDicom.ggo)
add_executable(sydInsertDicom sydInsertDicom.cxx sydInsertDicom_ggo.c)
target_link_libraries(sydInsertDicom sydStandardDatabase)

# Specific tool for DumpDicom
wrap_ggo(sydDumpDicom sydDumpDicom.ggo)
add_executable(sydDumpDicom sydDumpDicom.cxx sydDumpDicom_ggo.c)
target_link_libraries(sydDumpDicom sydStandardDatabase)

# Specific tool for Timepoint
wrap_ggo(sydInsertTimepoint sydInsertTimepoint.ggo)
add_executable(sydInsertTimepoint sydInsertTimepoint.cxx sydInsertTimepoint_ggo.c)
target_link_libraries(sydInsertTimepoint sydStandardDatabase)

# Specific tool for DumpTimepoint
wrap_ggo(sydDumpTimepoint sydDumpTimepoint.ggo)
add_executable(sydDumpTimepoint sydDumpTimepoint.cxx sydDumpTimepoint_ggo.c)
target_link_libraries(sydDumpTimepoint sydStandardDatabase)
#----------------------------------------------------------
