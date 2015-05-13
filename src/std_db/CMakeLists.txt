
#----------------------------------------------------------
# Standard database : ClinicDatabase
set(SCHEMA_NAME ClinicSchema)
wrap_odb(sydClinicDatabase sydPatient.h)
wrap_odb(sydClinicDatabase sydFile.h)
wrap_odb(sydClinicDatabase sydInjection.h)
wrap_odb(sydClinicDatabase sydDicomSerie.h)
wrap_odb(sydClinicDatabase sydDicomFile.h)

add_library(sydClinicDatabase SHARED
  sydClinicDatabase.cxx
  sydClinicDatabasePlugin.cxx
  sydPatient-odb.cxx sydPatient.cxx
  sydFile-odb.cxx sydFile.cxx
  sydInjection-odb.cxx sydInjection.cxx
  sydDicomSerie-odb.cxx sydDicomSerie.cxx
  sydDicomFile-odb.cxx sydDicomFile.cxx
  sydDicomSerieBuilder.cxx
  )
## to declare dependencies
target_link_libraries(sydClinicDatabase ${SYD_LIBS})

# Specific tool for DicomSerie
wrap_ggo(sydInsertDicom sydInsertDicom.ggo)
add_executable(sydInsertDicom sydInsertDicom.cxx sydInsertDicom_ggo.c)
target_link_libraries(sydInsertDicom sydClinicDatabase)

# Specific tool for DumpDicom
wrap_ggo(sydDumpDicom sydDumpDicom.ggo)
add_executable(sydDumpDicom sydDumpDicom.cxx sydDumpDicom_ggo.c)
target_link_libraries(sydDumpDicom sydClinicDatabase)
#----------------------------------------------------------


#----------------------------------------------------------
# Extended database trial (FIXME to be put elsewhere)
set(SCHEMA_NAME extClinicSchema)
wrap_odb(extClinicDatabase extPatient.h)
add_library(extClinicDatabase SHARED
  extClinicDatabase.cxx
  extClinicDatabasePlugin.cxx
  sydClinicDatabase.cxx
  extPatient-odb.cxx)
## to declare dependencies
target_link_libraries(extClinicDatabase sydClinicDatabase)

#----------------------------------------------------------


#----------------------------------------------------------
# Standard database : StudyDatabase
set(SCHEMA_NAME StudySchema)
wrap_odb(sydStudyDatabase sydTimepoint.h)
wrap_odb(sydStudyDatabase sydTag.h)

add_library(sydStudyDatabase SHARED
  sydStudyDatabase.cxx
  sydStudyDatabasePlugin.cxx
  sydTimepoint-odb.cxx sydTimepoint.cxx
  sydTag-odb.cxx sydTag.cxx
  sydTimepointBuilder.cxx
  )
## to declare dependencies
target_link_libraries(sydStudyDatabase sydClinicDatabase ${SYD_LIBS})

# Specific tool for Timepoint
wrap_ggo(sydInsertTimepoint sydInsertTimepoint.ggo)
add_executable(sydInsertTimepoint sydInsertTimepoint.cxx sydInsertTimepoint_ggo.c)
target_link_libraries(sydInsertTimepoint sydStudyDatabase)

# Specific tool for DumpTimepoint
wrap_ggo(sydDumpTimepoint sydDumpTimepoint.ggo)
add_executable(sydDumpTimepoint sydDumpTimepoint.cxx sydDumpTimepoint_ggo.c)
target_link_libraries(sydDumpTimepoint sydStudyDatabase)
#----------------------------------------------------------
