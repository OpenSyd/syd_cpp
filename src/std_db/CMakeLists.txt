

## Needed to find sydRecord-odb.hxx
#include_directories(${SYD_BINARY_DIR}/src/core)
#include_directories(${SYD_BINARY_DIR}/src/common_db)

# include_directories(../common_db)
# DD(SYD_SOURCE_DIR)

#----------------------------------------------------------
# Standard database : StandardDatabase
set(SCHEMA_NAME StandardDatabase)

wrap_odb(sydStandardDatabase sydPatient.h)
wrap_odb(sydStandardDatabase sydRadionuclide.h)
wrap_odb(sydStandardDatabase sydTag.h)
wrap_odb(sydStandardDatabase sydFile.h)
wrap_odb(sydStandardDatabase sydInjection.h)
wrap_odb(sydStandardDatabase sydDicomSerie.h)
wrap_odb(sydStandardDatabase sydDicomFile.h)
wrap_odb(sydStandardDatabase sydImage.h)
wrap_odb(sydStandardDatabase sydRoiType.h)
wrap_odb(sydStandardDatabase sydRoiMaskImage.h)
wrap_odb(sydStandardDatabase sydImageTransform.h)
wrap_odb(sydStandardDatabase sydCalibration.h)
wrap_odb(sydStandardDatabase sydPixelValueUnit.h)
wrap_odb(sydStandardDatabase sydRoiStatistic.h)

add_library(sydStandardDatabase SHARED
  sydStandardDatabase.cxx
  sydStandardDatabasePlugin.cxx
  sydPatient-odb.cxx sydPatient.cxx
  sydRadionuclide-odb.cxx sydRadionuclide.cxx
  sydTag-odb.cxx sydTag.cxx
  sydInjection-odb.cxx sydInjection.cxx
  sydFile-odb.cxx sydFile.cxx
  sydDicomSerie-odb.cxx sydDicomSerie.cxx
  sydDicomFile-odb.cxx sydDicomFile.cxx
  sydDicomSerieBuilder.cxx
  sydImage-odb.cxx sydImage.cxx
  sydRoiType-odb.cxx sydRoiType.cxx
  sydRoiMaskImage-odb.cxx sydRoiMaskImage.cxx
  sydImageTransform-odb.cxx sydImageTransform.cxx
  sydCalibration-odb.cxx sydCalibration.cxx
  sydPixelValueUnit-odb.cxx sydPixelValueUnit.cxx
  sydRoiStatistic-odb.cxx sydRoiStatistic.cxx

  sydImageBuilder.cxx
  sydFileBuilder.cxx

  sydImageFromDicomBuilder.cxx
  sydStitchDicomImageBuilder.cxx
  sydRoiMaskImageBuilder.cxx
  sydCropImageBuilder.cxx
  sydDecayCorrectedImageBuilder.cxx
  sydSubstituteRadionuclideImageBuilder.cxx
  sydRoiStatisticBuilder.cxx
  sydScaleImageBuilder.cxx

  )
## to declare dependencies
target_link_libraries(sydStandardDatabase ${SYD_CORE_LIBRARIES})

##############################
# Not really a tool, for debug only
wrap_ggo(sydTest_GGO_C sydTest.ggo sydCommandLineDefault.ggo)
add_executable(sydTest sydTest.cxx ${sydTest_GGO_C})
target_link_libraries(sydTest sydStandardDatabase)

##############################
# Specific tools (usable for StandardDatabase or db that inherit from it)


# Create DicomSeries
wrap_ggo(sydInsertDicom_GGO_C sydInsertDicom.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertDicom sydInsertDicom.cxx ${sydInsertDicom_GGO_C})
target_link_libraries(sydInsertDicom sydStandardDatabase)

# Create images
wrap_ggo(sydInsertImageFromDicom_GGO_C sydInsertImageFromDicom.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo )
add_executable(sydInsertImageFromDicom sydInsertImageFromDicom.cxx ${sydInsertImageFromDicom_GGO_C})
target_link_libraries(sydInsertImageFromDicom sydStandardDatabase)

# Create images
wrap_ggo(sydInsertImage_GGO_C sydInsertImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertImage sydInsertImage.cxx ${sydInsertImage_GGO_C})
target_link_libraries(sydInsertImage sydStandardDatabase)

# Update and check images
wrap_ggo(sydUpdateImage_GGO_C sydUpdateImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydUpdateImage sydUpdateImage.cxx ${sydUpdateImage_GGO_C})
target_link_libraries(sydUpdateImage sydStandardDatabase)

# Create images by stitching 2 dicom
wrap_ggo(sydStitchDicom_GGO_C sydStitchDicom.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydStitchDicom sydStitchDicom.cxx ${sydStitchDicom_GGO_C})
target_link_libraries(sydStitchDicom sydStandardDatabase)

# Dump content of the dicom
wrap_ggo(sydDicomInfo_GGO_C sydDicomInfo.ggo sydCommandLineDefault.ggo)
add_executable(sydDicomInfo sydDicomInfo.cxx ${sydDicomInfo_GGO_C})
target_link_libraries(sydDicomInfo sydStandardDatabase)

# Reduce the size of an image by cropping it (with a mask or with a threshold)
wrap_ggo(sydCropImage_GGO_C sydCropImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydCropImage sydCropImage.cxx ${sydCropImage_GGO_C})
target_link_libraries(sydCropImage sydStandardDatabase)

# Change tag
wrap_ggo(sydUpdateImageTag_GGO_C sydUpdateImageTag.ggo sydCommandLineDefault.ggo)
add_executable(sydUpdateImageTag sydUpdateImageTag.cxx ${sydUpdateImageTag_GGO_C})
target_link_libraries(sydUpdateImageTag sydStandardDatabase)

# Update Radionuclide table
wrap_ggo(sydUpdateRadionuclide_GGO_C sydUpdateRadionuclide.ggo sydCommandLineDefault.ggo)
add_executable(sydUpdateRadionuclide sydUpdateRadionuclide.cxx ${sydUpdateRadionuclide_GGO_C})
target_link_libraries(sydUpdateRadionuclide sydStandardDatabase)

# Insert / Update calibration for an image
wrap_ggo(sydInsertCalibration_GGO_C sydInsertCalibration.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertCalibration sydInsertCalibration.cxx ${sydInsertCalibration_GGO_C})
target_link_libraries(sydInsertCalibration sydStandardDatabase)

# Create decay corrected image
wrap_ggo(sydInsertDecayCorrectedImage_GGO_C sydInsertDecayCorrectedImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertDecayCorrectedImage sydInsertDecayCorrectedImage.cxx ${sydInsertDecayCorrectedImage_GGO_C})
target_link_libraries(sydInsertDecayCorrectedImage sydStandardDatabase)

# Create image with a different radionuclide
wrap_ggo(sydInsertSubstituteRadionuclideImage_GGO_C sydInsertSubstituteRadionuclideImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertSubstituteRadionuclideImage sydInsertSubstituteRadionuclideImage.cxx ${sydInsertSubstituteRadionuclideImage_GGO_C})
target_link_libraries(sydInsertSubstituteRadionuclideImage sydStandardDatabase)

# Insert a new RoiMaskImage
wrap_ggo(sydInsertRoiMaskImage_GGO_C sydInsertRoiMaskImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertRoiMaskImage sydInsertRoiMaskImage.cxx ${sydInsertRoiMaskImage_GGO_C})
target_link_libraries(sydInsertRoiMaskImage sydStandardDatabase)

# Insert a new RoiStatistic
wrap_ggo(sydInsertRoiStatistic_GGO_C sydInsertRoiStatistic.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertRoiStatistic sydInsertRoiStatistic.cxx ${sydInsertRoiStatistic_GGO_C})
target_link_libraries(sydInsertRoiStatistic sydStandardDatabase)

# Create Timepoints
# wrap_ggo(sydInsertTimepoint sydInsertTimepoint.ggo)
# add_executable(sydInsertTimepoint sydInsertTimepoint.cxx sydInsertTimepoint_GGO_C})
# target_link_libraries(sydInsertTimepoint sydStandardDatabase)

# Search for Timepoint
# wrap_ggo(sydDumpTimepoint sydDumpTimepoint.ggo)
# add_executable(sydDumpTimepoint sydDumpTimepoint.cxx sydDumpTimepoint_GGO_C})
# target_link_libraries(sydDumpTimepoint sydStandardDatabase)


# Create image of integrated activity
wrap_ggo(sydInsertIntegratedActivityImage_GGO_C sydInsertIntegratedActivityImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertIntegratedActivityImage sydInsertIntegratedActivityImage.cxx ${sydInsertIntegratedActivityImage_GGO_C})
target_link_libraries(sydInsertIntegratedActivityImage sydStandardDatabase)


#----------------------------------------------------------


#----------------------------------------------------------
# External tool: clitkExtractPatient
wrap_ggo(syd_clitkExtractPatient_GGO_C syd_clitkExtractPatient.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(syd_clitkExtractPatient syd_clitkExtractPatient.cxx ${syd_clitkExtractPatient_GGO_C})
target_link_libraries(syd_clitkExtractPatient sydStandardDatabase)


#----------------------------------------------------------
# External tool: elastix
wrap_ggo(syd_elastix_GGO_C syd_elastix.ggo sydCommandLineDefault.ggo)
add_executable(syd_elastix syd_elastix.cxx ${syd_elastix_GGO_C})
target_link_libraries(syd_elastix sydStandardDatabase )

#----------------------------------------------------------
# External tool: transformix
wrap_ggo(syd_transformix_GGO_C syd_transformix.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(syd_transformix syd_transformix.cxx ${syd_transformix_GGO_C})
target_link_libraries(syd_transformix sydStandardDatabase )
