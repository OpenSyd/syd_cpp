
#----------------------------------------------------------
# Standard database : StandardDatabase
set(SCHEMA_NAME StandardDatabaseSchema)
wrap_odb(sydStandardDatabase sydPatient.h)
wrap_odb(sydStandardDatabase sydRadionuclide.h)
wrap_odb(sydStandardDatabase sydFile.h)
wrap_odb(sydStandardDatabase sydInjection.h)
wrap_odb(sydStandardDatabase sydDicomSerie.h)
wrap_odb(sydStandardDatabase sydDicomFile.h)
wrap_odb(sydStandardDatabase sydTimepoint.h)
wrap_odb(sydStandardDatabase sydTag.h)
wrap_odb(sydStandardDatabase sydImage.h)

add_library(sydStandardDatabase SHARED
  sydStandardDatabase.cxx
  sydStandardDatabasePlugin.cxx
  sydPatient-odb.cxx sydPatient.cxx
  sydRadionuclide-odb.cxx sydRadionuclide.cxx
  sydFile-odb.cxx sydFile.cxx
  sydTag-odb.cxx sydTag.cxx
  sydInjection-odb.cxx sydInjection.cxx
  sydDicomSerie-odb.cxx sydDicomSerie.cxx
  sydDicomFile-odb.cxx sydDicomFile.cxx
  sydTimepoint-odb.cxx sydTimepoint.cxx
  sydImage-odb.cxx sydImage.cxx
  sydDicomSerieBuilder.cxx
  sydTimepointBuilder.cxx
  sydImageBuilder.cxx
  )
## to declare dependencies
target_link_libraries(sydStandardDatabase ${SYD_CORE_LIBRARIES})

# Create DicomSeries
wrap_ggo(sydInsertDicom sydInsertDicom.ggo)
add_executable(sydInsertDicom sydInsertDicom.cxx sydInsertDicom_ggo.c)
target_link_libraries(sydInsertDicom sydStandardDatabase)

# Search for Dicom
wrap_ggo(sydDumpDicom sydDumpDicom.ggo)
add_executable(sydDumpDicom sydDumpDicom.cxx sydDumpDicom_ggo.c)
target_link_libraries(sydDumpDicom sydStandardDatabase)

# Create Timepoints
wrap_ggo(sydInsertTimepoint sydInsertTimepoint.ggo)
add_executable(sydInsertTimepoint sydInsertTimepoint.cxx sydInsertTimepoint_ggo.c)
target_link_libraries(sydInsertTimepoint sydStandardDatabase)

# Search for Timepoint
wrap_ggo(sydDumpTimepoint sydDumpTimepoint.ggo)
add_executable(sydDumpTimepoint sydDumpTimepoint.cxx sydDumpTimepoint_ggo.c)
target_link_libraries(sydDumpTimepoint sydStandardDatabase)

# Search for Timepoint
wrap_ggo(sydInsertImage sydInsertImage.ggo)
add_executable(sydInsertImage sydInsertImage.cxx sydInsertImage_ggo.c)
target_link_libraries(sydInsertImage sydStandardDatabase)
#----------------------------------------------------------
