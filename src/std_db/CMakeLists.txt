

## Needed to find sydRecord-odb.hxx
include_directories(${SYD_BINARY_DIR}/src/core)


#----------------------------------------------------------
# Standard database : StandardDatabase
set(SCHEMA_NAME StandardDatabase)
wrap_odb(sydStandardDatabase sydPatient.h)
wrap_odb(sydStandardDatabase sydRadionuclide.h)
wrap_odb(sydStandardDatabase sydTag.h)
# wrap_odb(sydStandardDatabase sydFile.h)
wrap_odb(sydStandardDatabase sydInjection.h)
# wrap_odb(sydStandardDatabase sydDicomSerie.h)
# wrap_odb(sydStandardDatabase sydDicomFile.h)
# #wrap_odb(sydStandardDatabase sydTimepoint.h)
# wrap_odb(sydStandardDatabase sydImage.h)
# wrap_odb(sydStandardDatabase sydRoiType.h)
# wrap_odb(sydStandardDatabase sydRoiMaskImage.h)
# wrap_odb(sydStandardDatabase sydImageTransform.h)

add_library(sydStandardDatabase SHARED
  sydStandardDatabase.cxx
  sydStandardDatabasePlugin.cxx
  sydPatient-odb.cxx sydPatient.cxx
  sydRadionuclide-odb.cxx sydRadionuclide.cxx
  sydTag-odb.cxx sydTag.cxx
  sydInjection-odb.cxx sydInjection.cxx
#   sydFile-odb.cxx sydFile.cxx
#   sydDicomSerie-odb.cxx sydDicomSerie.cxx
#   sydDicomFile-odb.cxx sydDicomFile.cxx
# #  sydTimepoint-odb.cxx sydTimepoint.cxx
#   sydImage-odb.cxx sydImage.cxx
#   sydImageTransform-odb.cxx sydImageTransform.cxx
#   sydRoiType-odb.cxx sydRoiType.cxx
#   sydRoiMaskImage-odb.cxx sydRoiMaskImage.cxx
#   sydDicomSerieBuilder.cxx
# #  sydTimepointBuilder.cxx
#   sydImageBuilder.cxx

  # sydTablePatient.cxx
  # sydTableTag.h
  # sydTableRoiType.h
  # sydTableFile.cxx
  # sydTableDicomFile.cxx
  # sydTableDicomSerie.cxx
  # sydTableImage.cxx
  # sydTableRoiMaskImage.cxx
  # sydTableImageTransform.cxx
  )
## to declare dependencies
target_link_libraries(sydStandardDatabase ${SYD_CORE_LIBRARIES})


##############################
# # Not really a tool, for debug only
# wrap_ggo(sydTest sydTest.ggo)
# add_executable(sydTest sydTest.cxx sydTest_ggo.c)
# target_link_libraries(sydTest extExtendedDatabase sydStandardDatabase)

##############################
# Specific tools (usable for StandardDatabase or db that inherit from it)


# # Create DicomSeries
# wrap_ggo(sydInsertDicom sydInsertDicom.ggo)
# add_executable(sydInsertDicom sydInsertDicom.cxx sydInsertDicom_ggo.c)
# target_link_libraries(sydInsertDicom sydStandardDatabase)

# # Create images
# wrap_ggo(sydInsertImageFromDicom sydInsertImageFromDicom.ggo)
# add_executable(sydInsertImageFromDicom sydInsertImageFromDicom.cxx sydInsertImageFromDicom_ggo.c)
# target_link_libraries(sydInsertImageFromDicom sydStandardDatabase)

# # Create images by stitching 2 dicom
# wrap_ggo(sydStitchDicom sydStitchDicom.ggo)
# add_executable(sydStitchDicom sydStitchDicom.cxx sydStitchDicom_ggo.c)
# target_link_libraries(sydStitchDicom sydStandardDatabase)

# # Reduce the size of an image by cropping it (with a mask or with a threshold)
# wrap_ggo(sydCropImage sydCropImage.ggo)
# add_executable(sydCropImage sydCropImage.cxx sydCropImage_ggo.c)
# target_link_libraries(sydCropImage sydStandardDatabase)

# # Change tag
# wrap_ggo(sydImageTag sydImageTag.ggo)
# add_executable(sydImageTag sydImageTag.cxx sydImageTag_ggo.c)
# target_link_libraries(sydImageTag sydStandardDatabase)

# # Insert a new RoiMaskImage
# wrap_ggo(sydInsertRoiMaskImage sydInsertRoiMaskImage.ggo)
# add_executable(sydInsertRoiMaskImage sydInsertRoiMaskImage.cxx sydInsertRoiMaskImage_ggo.c)
# target_link_libraries(sydInsertRoiMaskImage sydStandardDatabase)

# Create Timepoints
# wrap_ggo(sydInsertTimepoint sydInsertTimepoint.ggo)
# add_executable(sydInsertTimepoint sydInsertTimepoint.cxx sydInsertTimepoint_ggo.c)
# target_link_libraries(sydInsertTimepoint sydStandardDatabase)

# Search for Timepoint
# wrap_ggo(sydDumpTimepoint sydDumpTimepoint.ggo)
# add_executable(sydDumpTimepoint sydDumpTimepoint.cxx sydDumpTimepoint_ggo.c)
# target_link_libraries(sydDumpTimepoint sydStandardDatabase)

#----------------------------------------------------------


# #----------------------------------------------------------
# # External tool: clitkExtractPatient
# wrap_ggo(syd_clitkExtractPatient syd_clitkExtractPatient.ggo)
# add_executable(syd_clitkExtractPatient syd_clitkExtractPatient.cxx syd_clitkExtractPatient_ggo.c)
# target_link_libraries(syd_clitkExtractPatient sydStandardDatabase)


# #----------------------------------------------------------
# # External tool: elastix
# wrap_ggo(syd_elastix syd_elastix.ggo)
# add_executable(syd_elastix syd_elastix.cxx syd_elastix_ggo.c)
# target_link_libraries(syd_elastix sydStandardDatabase)
