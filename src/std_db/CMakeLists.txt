

#----------------------------------------------------------
# Standard database : StandardDatabase
set(SCHEMA_NAME StandardDatabase)

wrap_odb(sydStandardDatabase sydPatient.h) ## <--- to put in common_db ?
wrap_odb(sydStandardDatabase sydRadionuclide.h) ## in common_db ?
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
wrap_odb(sydStandardDatabase sydTimepoints.h)
wrap_odb(sydStandardDatabase sydFitResult.h)

set(SYD_STANDARD_DATABASE_SRC
  sydStandardDatabase.cxx
  sydPatient-odb.cxx sydPatient-schema.cxx  sydPatient.cxx
  sydRadionuclide-odb.cxx sydRadionuclide-schema.cxx  sydRadionuclide.cxx
  sydInjection-odb.cxx sydInjection-schema.cxx  sydInjection.cxx
  sydDicomSerie-odb.cxx sydDicomSerie-schema.cxx  sydDicomSerie.cxx
  sydDicomFile-odb.cxx sydDicomFile-schema.cxx  sydDicomFile.cxx
  sydImage-odb.cxx sydImage-schema.cxx  sydImage.cxx
  sydRoiType-odb.cxx sydRoiType-schema.cxx  sydRoiType.cxx
  sydRoiMaskImage-odb.cxx sydRoiMaskImage-schema.cxx  sydRoiMaskImage.cxx
  sydImageTransform-odb.cxx sydImageTransform-schema.cxx  sydImageTransform.cxx
  sydCalibration-odb.cxx sydCalibration-schema.cxx  sydCalibration.cxx
  sydPixelValueUnit-odb.cxx sydPixelValueUnit-schema.cxx  sydPixelValueUnit.cxx
  sydRoiStatistic-odb.cxx sydRoiStatistic-schema.cxx  sydRoiStatistic.cxx
  sydTimepoints-odb.cxx sydTimepoints-schema.cxx  sydTimepoints.cxx
  sydFitResult-odb.cxx sydFitResult-schema.cxx  sydFitResult.cxx
  sydDicomSerieBuilder.cxx
  sydImageBuilder.cxx
  sydFileBuilder.cxx
  sydImageFromDicomBuilder.cxx
  sydStitchDicomImageBuilder.cxx
  sydRoiMaskImageBuilder.cxx
  sydCropImageBuilder.cxx
  sydDecayCorrectedImageBuilder.cxx
  sydSubstituteRadionuclideImageBuilder.cxx
  sydTimeIntegratedActivityImageBuilder.cxx
  sydRoiStatisticBuilder.cxx
  sydScaleImageBuilder.cxx
  sydUpdateRadionuclideFilter.cxx
  sydTimepointsBuilder.cxx
  )

# Group all objects
add_library(sydStandardDatabaseObjects OBJECT ${SYD_STANDARD_DATABASE_SRC})

# Conventional shared lib for command line executables
add_library(StandardDatabase SHARED
  sydStandardDatabaseStatic.cxx
  $<TARGET_OBJECTS:sydStandardDatabaseObjects>
  )
target_link_libraries(StandardDatabase sydCommonDatabase ${SYD_CORE_LIBRARIES})

# Plugin library : MUST be called lib<DatabaseType>Plugin.so
add_library(StandardDatabasePlugin MODULE
  sydStandardDatabasePlugin.cxx
  $<TARGET_OBJECTS:sydStandardDatabaseObjects>)
target_link_libraries(StandardDatabasePlugin sydCommonDatabase ${SYD_CORE_LIBRARIES})
set_target_properties(StandardDatabasePlugin PROPERTIES MACOSX_BUNDLE TRUE)
set_target_properties(StandardDatabasePlugin PROPERTIES BUNDLE_EXTENSION ".so")

##############################
# Not really a tool, for debug only
wrap_ggo(sydTest_GGO_C sydTest.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydTest sydTest.cxx ${sydTest_GGO_C})
target_link_libraries(sydTest StandardDatabase)

##############################
# Image related tool
##############################

# Create images
wrap_ggo(sydInsertImageFromDicom_GGO_C
  sydInsertImageFromDicom.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo )
add_executable(sydInsertImageFromDicom sydInsertImageFromDicom.cxx ${sydInsertImageFromDicom_GGO_C})
target_link_libraries(sydInsertImageFromDicom StandardDatabase)

# Create images
wrap_ggo(sydInsertImage_GGO_C
  sydInsertImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertImage sydInsertImage.cxx ${sydInsertImage_GGO_C})
target_link_libraries(sydInsertImage StandardDatabase)

# Copy an image
wrap_ggo(sydCopyImage_GGO_C
  sydCopyImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydCopyImage sydCopyImage.cxx ${sydCopyImage_GGO_C})
target_link_libraries(sydCopyImage StandardDatabase)

# Update image
wrap_ggo(sydUpdateImage_GGO_C
  sydUpdateImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydUpdateImage sydUpdateImage.cxx ${sydUpdateImage_GGO_C})
target_link_libraries(sydUpdateImage StandardDatabase)

# Multiply image
wrap_ggo(sydInsertMultiplyImage_GGO_C
  sydInsertMultiplyImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertMultiplyImage sydInsertMultiplyImage.cxx ${sydInsertMultiplyImage_GGO_C})
target_link_libraries(sydInsertMultiplyImage StandardDatabase)

# Insert a new image with a calibration
wrap_ggo(sydInsertCalibratedImage_GGO_C
  sydInsertCalibratedImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertCalibratedImage sydInsertCalibratedImage.cxx ${sydInsertCalibratedImage_GGO_C})
target_link_libraries(sydInsertCalibratedImage StandardDatabase)

# Update a dose image
wrap_ggo(sydUpdateDoseImage_GGO_C
  sydUpdateDoseImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydUpdateDoseImage sydUpdateDoseImage.cxx ${sydUpdateDoseImage_GGO_C})
target_link_libraries(sydUpdateDoseImage StandardDatabase)

# Create images by stitching 2 dicom
wrap_ggo(sydStitchDicom_GGO_C
  sydStitchDicom.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydStitchDicom sydStitchDicom.cxx ${sydStitchDicom_GGO_C})
target_link_libraries(sydStitchDicom StandardDatabase)

# Reduce the size of an image by cropping it (with a mask or with a threshold)
wrap_ggo(sydCropImage_GGO_C sydCropImage.ggo
  sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydCropImage sydCropImage.cxx ${sydCropImage_GGO_C})
target_link_libraries(sydCropImage StandardDatabase)

# Create decay corrected image
wrap_ggo(sydInsertDecayCorrectedImage_GGO_C
  sydInsertDecayCorrectedImage.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertDecayCorrectedImage sydInsertDecayCorrectedImage.cxx ${sydInsertDecayCorrectedImage_GGO_C})
target_link_libraries(sydInsertDecayCorrectedImage StandardDatabase)

# Create image with a different radionuclide
wrap_ggo(sydInsertSubstituteRadionuclideImage_GGO_C
  sydInsertSubstituteRadionuclideImage.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertSubstituteRadionuclideImage
  sydInsertSubstituteRadionuclideImage.cxx ${sydInsertSubstituteRadionuclideImage_GGO_C})
target_link_libraries(sydInsertSubstituteRadionuclideImage StandardDatabase)

# Insert a new RoiMaskImage
wrap_ggo(sydInsertRoiMaskImage_GGO_C
  sydInsertRoiMaskImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertRoiMaskImage sydInsertRoiMaskImage.cxx ${sydInsertRoiMaskImage_GGO_C})
target_link_libraries(sydInsertRoiMaskImage StandardDatabase)

# Create image of integrated activity
wrap_ggo(sydInsertTimeIntegratedActivityImage_GGO_C
  sydInsertTimeIntegratedActivityImage.ggo
  sydCommandLineWithFitOptions.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertTimeIntegratedActivityImage
  sydInsertTimeIntegratedActivityImage.cxx ${sydInsertTimeIntegratedActivityImage_GGO_C})
target_link_libraries(sydInsertTimeIntegratedActivityImage StandardDatabase)

# compute integrated activity in a ROI
wrap_ggo(sydInsertFitResult_GGO_C
  sydInsertFitResult.ggo
  sydCommandLineWithFitOptions.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertFitResult
  sydInsertFitResult.cxx ${sydInsertFitResult_GGO_C})
target_link_libraries(sydInsertFitResult StandardDatabase)


# insert a timepoints from a roi / image
wrap_ggo(sydInsertTimepoints_GGO_C sydInsertTimepoints.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertTimepoints sydInsertTimepoints.cxx ${sydInsertTimepoints_GGO_C})
target_link_libraries(sydInsertTimepoints StandardDatabase)


# insert a timepoints from a pixel coord
wrap_ggo(sydInsertPixelTimepoints_GGO_C sydInsertPixelTimepoints.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertPixelTimepoints sydInsertPixelTimepoints.cxx ${sydInsertPixelTimepoints_GGO_C})
target_link_libraries(sydInsertPixelTimepoints StandardDatabase)


# Create images
wrap_ggo(sydInsertPlanarGeometricalMean_GGO_C
  sydInsertPlanarGeometricalMean.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo )
add_executable(sydInsertPlanarGeometricalMean
  sydInsertPlanarGeometricalMean.cxx ${sydInsertPlanarGeometricalMean_GGO_C})
target_link_libraries(sydInsertPlanarGeometricalMean StandardDatabase)



##############################
# Other tools
##############################


# Create DicomSeries
wrap_ggo(sydInsertDicom_GGO_C sydInsertDicom.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertDicom sydInsertDicom.cxx ${sydInsertDicom_GGO_C})
target_link_libraries(sydInsertDicom StandardDatabase)

# Update DicomSerie
wrap_ggo(sydUpdateDicomSerie_GGO_C
  sydUpdateDicomSerie.ggo sydCommandLineDefault.ggo)
add_executable(sydUpdateDicomSerie sydUpdateDicomSerie.cxx ${sydUpdateDicomSerie_GGO_C})
target_link_libraries(sydUpdateDicomSerie StandardDatabase)

# Dump content of the dicom
wrap_ggo(sydDicomInfo_GGO_C sydDicomInfo.ggo sydCommandLineDefault.ggo)
add_executable(sydDicomInfo sydDicomInfo.cxx ${sydDicomInfo_GGO_C})
target_link_libraries(sydDicomInfo StandardDatabase)

# Update Radionuclide table
wrap_ggo(sydUpdateRadionuclide_GGO_C sydUpdateRadionuclide.ggo sydCommandLineDefault.ggo)
add_executable(sydUpdateRadionuclide sydUpdateRadionuclide.cxx ${sydUpdateRadionuclide_GGO_C})
target_link_libraries(sydUpdateRadionuclide StandardDatabase)

# Insert / Update calibration for an image
wrap_ggo(sydInsertCalibration_GGO_C sydInsertCalibration.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertCalibration sydInsertCalibration.cxx ${sydInsertCalibration_GGO_C})
target_link_libraries(sydInsertCalibration StandardDatabase)


# Insert a new RoiStatistic
wrap_ggo(sydInsertRoiStatistic_GGO_C sydInsertRoiStatistic.ggo sydCommandLineWithTags.ggo  sydCommandLineDefault.ggo)
add_executable(sydInsertRoiStatistic sydInsertRoiStatistic.cxx ${sydInsertRoiStatistic_GGO_C})
target_link_libraries(sydInsertRoiStatistic StandardDatabase)

#----------------------------------------------------------
#----------------------------------------------------------
# External tool: clitkExtractPatient
wrap_ggo(syd_clitkExtractPatient_GGO_C
  syd_clitkExtractPatient.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(syd_clitkExtractPatient
  syd_clitkExtractPatient.cxx ${syd_clitkExtractPatient_GGO_C})
target_link_libraries(syd_clitkExtractPatient StandardDatabase)


#----------------------------------------------------------
# External tool: elastix
wrap_ggo(syd_elastix_GGO_C syd_elastix.ggo sydCommandLineDefault.ggo)
add_executable(syd_elastix syd_elastix.cxx ${syd_elastix_GGO_C})
target_link_libraries(syd_elastix StandardDatabase )

#----------------------------------------------------------
# External tool: transformix
wrap_ggo(syd_transformix_GGO_C
  syd_transformix.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(syd_transformix syd_transformix.cxx ${syd_transformix_GGO_C})
target_link_libraries(syd_transformix StandardDatabase )


#----------------------------------------------------------
# External link Gate
wrap_ggo(sydInsertGateOutput sydInsertGateOutput.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertGateOutput sydInsertGateOutput.cxx sydInsertGateOutput_ggo.c)
target_link_libraries(sydInsertGateOutput StandardDatabase)

#----------------------------------------------------------
# Compute RootMeanSquareError in a mask
wrap_ggo(sydInsertRootMeanSquareImage_GGO_C
  sydInsertRootMeanSquareImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydInsertRootMeanSquareImage sydInsertRootMeanSquareImage.cxx ${sydInsertRootMeanSquareImage_GGO_C})
target_link_libraries(sydInsertRootMeanSquareImage StandardDatabase)


#----------------------------------------------------------
# Kmeans
wrap_ggo(sydKmeansBuildInputData_GGO_C
  sydKmeansBuildInputData.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydKmeansBuildInputData sydKmeansInputDataBuilder.cxx
  sydKmeansBuildInputData.cxx ${sydKmeansBuildInputData_GGO_C})
target_link_libraries(sydKmeansBuildInputData StandardDatabase)

wrap_ggo(sydKmeans_GGO_C
  sydKmeans.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydKmeans sydKmeans.cxx ${sydKmeans_GGO_C})
target_link_libraries(sydKmeans StandardDatabase)
