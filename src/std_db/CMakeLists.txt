
#----------------------------------------------------------
# Standard database : StandardDatabase
set(SCHEMA_NAME StandardDatabase)

wrap_odb(sydStandardDatabase sydPatient.h)
wrap_odb(sydStandardDatabase sydRadionuclide.h)
wrap_odb(sydStandardDatabase sydInjection.h)
wrap_odb(sydStandardDatabase sydDicomSerie.h)
wrap_odb(sydStandardDatabase sydDicomFile.h)
wrap_odb(sydStandardDatabase sydPixelUnit.h)
wrap_odb(sydStandardDatabase sydImage.h)
wrap_odb(sydStandardDatabase sydRoiType.h)
wrap_odb(sydStandardDatabase sydRoiMaskImage.h)
wrap_odb(sydStandardDatabase sydRoiStatistic.h)
wrap_odb(sydStandardDatabase sydElastix.h)
wrap_odb(sydStandardDatabase sydTimepoints.h)
wrap_odb(sydStandardDatabase sydRoiTimepoints.h)
wrap_odb(sydStandardDatabase sydFitOptions.h)
wrap_odb(sydStandardDatabase sydFitImages.h)
wrap_odb(sydStandardDatabase sydFitTimepoints.h)

set(SYD_STANDARD_DATABASE_SRC
  sydRadionuclide-odb.cxx sydRadionuclide-schema.cxx sydRadionuclide.cxx
  sydPatient-odb.cxx sydPatient-schema.cxx  sydPatient.cxx
  sydInjection-odb.cxx sydInjection-schema.cxx  sydInjection.cxx
  sydDicomSerie-odb.cxx sydDicomSerie-schema.cxx  sydDicomSerie.cxx
  sydDicomFile-odb.cxx sydDicomFile-schema.cxx  sydDicomFile.cxx
  sydPixelUnit-odb.cxx sydPixelUnit-schema.cxx sydPixelUnit.cxx
  sydImage-odb.cxx sydImage-schema.cxx  sydImage.cxx
  sydRoiType-odb.cxx sydRoiType-schema.cxx sydRoiType.cxx
  sydRoiMaskImage-odb.cxx sydRoiMaskImage-schema.cxx sydRoiMaskImage.cxx
  sydRoiStatistic-odb.cxx sydRoiStatistic-schema.cxx  sydRoiStatistic.cxx

  sydElastix-odb.cxx sydElastix-schema.cxx sydElastix.cxx

  sydFitImages-odb.cxx sydFitImages-schema.cxx sydFitImages.cxx
  sydFitOptions-odb.cxx sydFitOptions-schema.cxx sydFitOptions.cxx
  sydTimepoints-odb.cxx sydTimepoints-schema.cxx sydTimepoints.cxx
  sydRoiTimepoints-odb.cxx sydRoiTimepoints-schema.cxx sydRoiTimepoints.cxx
  sydFitTimepoints-odb.cxx sydFitTimepoints-schema.cxx sydFitTimepoints.cxx

  sydDicomSerieTraits.cxx
  sydImageTraits.cxx
  sydRoiMaskImageTraits.cxx
  sydInjectionTraits.cxx
  sydPatientTraits.cxx
  sydRadionuclideTraits.cxx
  sydStandardDatabase.cxx
  sydDicomFileTraits.cxx
  sydPixelUnitTraits.cxx
  sydRoiTypeTraits.cxx
  sydRoiStatisticTraits.cxx
  sydFitImagesTraits.cxx
  sydTimepointsTraits.cxx

  sydPixelUnitHelper.cxx
  sydDicomSerieBuilder.cxx
  sydDicomSerieHelper.cxx
  sydImageHelper.cxx
  sydInjectionHelper.cxx
  sydRoiMaskImageHelper.cxx
  sydUpdateRadionuclideFilter.cxx
  sydElastixHelper.cxx
  sydRadionuclideHelper.cxx
  sydRoiStatisticHelper.cxx
  sydTimepointsHelper.cxx
  sydFitImagesHelper.cxx

  ## Cannot put this file in the lib, because the plugin will be no more
  ## recognized. I dont know why.
  #sydTimeIntegratedActivityImageBuilder.cxx

  #  sydCropImageBuilder.cxx
  #  sydDecayCorrectedImageBuilder.cxx
  #  sydSubstituteRadionuclideImageBuilder.cxx
  )

# Group all objects
add_library(sydStandardDatabaseObjects OBJECT ${SYD_STANDARD_DATABASE_SRC})

# Conventional shared lib for command line executables
add_library(sydStandardDatabase SHARED
  sydStandardDatabaseStatic.cxx
  $<TARGET_OBJECTS:sydStandardDatabaseObjects>)

# Plugin library : MUST be called lib<DatabaseType>Plugin.so
add_library(StandardDatabasePlugin MODULE
  sydStandardDatabasePlugin.cxx
  $<TARGET_OBJECTS:sydStandardDatabaseObjects>)

if (APPLE)
  target_link_libraries(sydStandardDatabase ${SYD_COMMON_DB_LIBRARY})
  target_link_libraries(StandardDatabasePlugin ${SYD_COMMON_DB_LIBRARY})
else(APPLE)
  target_link_libraries(StandardDatabasePlugin sydCore sydFit sydItkUtils ${ITK_LIBRARIES})
endif(APPLE)

set_target_properties(StandardDatabasePlugin PROPERTIES MACOSX_BUNDLE TRUE)
set_target_properties(StandardDatabasePlugin PROPERTIES BUNDLE_EXTENSION ".so")

##############################
# Not really a tool, for debug only
wrap_ggo(sydTest_GGO_C sydTest.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydTest sydTableOfRecords.cxx sydTest.cxx ${sydTest_GGO_C})
target_link_libraries(sydTest ${SYD_STD_DB_LIBRARY})


# -----------------------------------------------------------------------------
# Command line tools
# -----------------------------------------------------------------------------

# Create DicomSeries
wrap_ggo(sydInsertDicom_GGO_C
  sydInsertDicom.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertDicom
  sydInsertDicom.cxx
  ${sydInsertDicom_GGO_C})
target_link_libraries(sydInsertDicom ${SYD_STD_DB_LIBRARY})

# Dump content of the dicom
wrap_ggo(sydDicomInfo_GGO_C
  sydDicomInfo.ggo
  sydCommandLineDefault.ggo)
add_executable(sydDicomInfo
  sydDicomInfo.cxx
  ${sydDicomInfo_GGO_C})
target_link_libraries(sydDicomInfo ${SYD_STD_DB_LIBRARY})

# Create anonymized dicom
wrap_ggo(sydInsertAnonymizedDicom_GGO_C
  sydInsertAnonymizedDicom.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertAnonymizedDicom
  sydInsertAnonymizedDicom.cxx
  ${sydInsertAnonymizedDicom_GGO_C})
target_link_libraries(sydInsertAnonymizedDicom ${SYD_STD_DB_LIBRARY})


# Update Radionuclide table
wrap_ggo(sydUpdateRadionuclide_GGO_C
  sydUpdateRadionuclide.ggo
  sydCommandLineDefault.ggo)
add_executable(sydUpdateRadionuclide
  sydUpdateRadionuclide.cxx
  ${sydUpdateRadionuclide_GGO_C})
target_link_libraries(sydUpdateRadionuclide ${SYD_STD_DB_LIBRARY})

# Create images
wrap_ggo(sydInsertImage_GGO_C
  sydInsertImage.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertImage
  sydInsertImage.cxx
  ${sydInsertImage_GGO_C})
target_link_libraries(sydInsertImage ${SYD_STD_DB_LIBRARY})

# Create images
wrap_ggo(sydInsertImageFromDicom_GGO_C
  sydInsertImageFromDicom.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo )
add_executable(sydInsertImageFromDicom
  sydInsertImageFromDicom.cxx
  ${sydInsertImageFromDicom_GGO_C})
target_link_libraries(sydInsertImageFromDicom ${SYD_STD_DB_LIBRARY})

# Update image
wrap_ggo(sydUpdateImage_GGO_C
  sydUpdateImage.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydUpdateImage
  sydUpdateImage.cxx
  ${sydUpdateImage_GGO_C})
target_link_libraries(sydUpdateImage ${SYD_STD_DB_LIBRARY})

# Create images by stitching 2 dicom
wrap_ggo(sydStitchDicom_GGO_C
  sydStitchDicom.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydStitchDicom
  sydStitchDicom.cxx
  ${sydStitchDicom_GGO_C})
target_link_libraries(sydStitchDicom ${SYD_STD_DB_LIBRARY})

# Create image with geometrical mean from 4 planar images
wrap_ggo(sydInsertPlanarGeometricalMean_GGO_C
  sydInsertPlanarGeometricalMean.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo )
add_executable(sydInsertPlanarGeometricalMean
  sydInsertPlanarGeometricalMean.cxx
  ${sydInsertPlanarGeometricalMean_GGO_C})
target_link_libraries(sydInsertPlanarGeometricalMean ${SYD_STD_DB_LIBRARY})

# Insert a new RoiMaskImage
wrap_ggo(sydInsertRoiMaskImage_GGO_C
  sydInsertRoiMaskImage.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertRoiMaskImage
  sydInsertRoiMaskImage.cxx
  ${sydInsertRoiMaskImage_GGO_C})
target_link_libraries(sydInsertRoiMaskImage ${SYD_STD_DB_LIBRARY})

# Insert a new RoiStatistic
wrap_ggo(sydInsertRoiStatistic_GGO_C
  sydInsertRoiStatistic.ggo
  sydCommandLineWithTags.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertRoiStatistic
  sydInsertRoiStatistic.cxx
  ${sydInsertRoiStatistic_GGO_C})
target_link_libraries(sydInsertRoiStatistic ${SYD_STD_DB_LIBRARY})


# ComputeActivityInMBqByDetectedCounts
wrap_ggo(sydComputeActivityInMBqByDetectedCounts_GGO_C
  sydComputeActivityInMBqByDetectedCounts.ggo
  sydCommandLineDefault.ggo)
add_executable(sydComputeActivityInMBqByDetectedCounts
  sydComputeActivityInMBqByDetectedCounts.cxx
  ${sydComputeActivityInMBqByDetectedCounts_GGO_C})
target_link_libraries(sydComputeActivityInMBqByDetectedCounts ${SYD_STD_DB_LIBRARY})


# Calibrate image
wrap_ggo(sydCalibrateImage_GGO_C
  sydCalibrateImage.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydCalibrateImage
  sydCalibrateImage.cxx
  ${sydCalibrateImage_GGO_C})
target_link_libraries(sydCalibrateImage ${SYD_STD_DB_LIBRARY})


# Average image
wrap_ggo(sydInsertAveragedImages_GGO_C
  sydInsertAveragedImages.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertAveragedImages
  sydInsertAveragedImages.cxx
  ${sydInsertAveragedImages_GGO_C})
target_link_libraries(sydInsertAveragedImages ${SYD_STD_DB_LIBRARY})


# Substitute a radionuclide
wrap_ggo(sydSubstituteRadionuclide_GGO_C
  sydSubstituteRadionuclide.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  sydCommandLineDefault.ggo)
add_executable(sydSubstituteRadionuclide
  sydSubstituteRadionuclide.cxx
  ${sydSubstituteRadionuclide_GGO_C})
target_link_libraries(sydSubstituteRadionuclide ${SYD_STD_DB_LIBRARY})


# Create image of integrated activity
wrap_ggo(sydInsertTimeIntegratedActivityImage_GGO_C
  sydInsertTimeIntegratedActivityImage.ggo
  sydCommandLineWithFitOptions.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertTimeIntegratedActivityImage
  sydInsertTimeIntegratedActivityImage.cxx
  sydTimeIntegratedActivityImageBuilder.cxx
  ${sydInsertTimeIntegratedActivityImage_GGO_C})
target_link_libraries(sydInsertTimeIntegratedActivityImage ${SYD_STD_DB_LIBRARY})

# Create image of integrated activity
wrap_ggo(sydInsertRoiBasedTimeIntegratedActivity_GGO_C
  sydInsertRoiBasedTimeIntegratedActivity.ggo
  sydCommandLineWithTags.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertRoiBasedTimeIntegratedActivity
  sydInsertRoiBasedTimeIntegratedActivity.cxx
  sydTimeIntegratedActivityImageBuilder.cxx
  ${sydInsertRoiBasedTimeIntegratedActivity_GGO_C})
target_link_libraries(sydInsertRoiBasedTimeIntegratedActivity ${SYD_STD_DB_LIBRARY})

#----------------------------------------------------------
# External tool: clitkExtractPatient
wrap_ggo(syd_clitkExtractPatient_GGO_C
  syd_clitkExtractPatient.ggo
  sydCommandLineWithTags.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineWithImageInfo.ggo
  sydCommandLineDefault.ggo)
add_executable(syd_clitkExtractPatient
  syd_clitkExtractPatient.cxx ${syd_clitkExtractPatient_GGO_C})
target_link_libraries(syd_clitkExtractPatient ${SYD_STD_DB_LIBRARY})

wrap_ggo(sydInsertElastixRegistration_GGO_C
  sydInsertElastixRegistration.ggo
  sydCommandLineWithTags.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertElastixRegistration
  sydInsertElastixRegistration.cxx
  ${sydInsertElastixRegistration_GGO_C})
target_link_libraries(sydInsertElastixRegistration ${SYD_STD_DB_LIBRARY})

wrap_ggo(syd_elastix_GGO_C
  syd_elastix.ggo
  sydCommandLineDefault.ggo)
add_executable(syd_elastix
  syd_elastix.cxx
  ${syd_elastix_GGO_C})
target_link_libraries(syd_elastix ${SYD_STD_DB_LIBRARY})

wrap_ggo(syd_transformix_GGO_C
  syd_transformix.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(syd_transformix
  syd_transformix.cxx
  ${syd_transformix_GGO_C})
target_link_libraries(syd_transformix ${SYD_STD_DB_LIBRARY})

# -----------------------------------------------------
# -----------------------------------------------------
# -----------------------------------------------------


# Update DicomSerie
# wrap_ggo(sydUpdateDicomSerie_GGO_C
#   sydUpdateDicomSerie.ggo
#   sydCommandLineDefault.ggo)
# add_executable(sydUpdateDicomSerie
#   sydUpdateDicomSerie.cxx
#   ${sydUpdateDicomSerie_GGO_C})
# target_link_libraries(sydUpdateDicomSerie ${SYD_STD_DB_LIBRARY})

# Update a dose image
# wrap_ggo(sydUpdateDoseImage_GGO_C
#   sydUpdateDoseImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
# add_executable(sydUpdateDoseImage sydUpdateDoseImage.cxx ${sydUpdateDoseImage_GGO_C})
# target_link_libraries(sydUpdateDoseImage ${SYD_STD_DB_LIBRARY})

# Reduce the size of an image by cropping it (with a mask or with a threshold)
# wrap_ggo(sydCropImage_GGO_C sydCropImage.ggo
#   sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
# add_executable(sydCropImage sydCropImage.cxx ${sydCropImage_GGO_C})
# target_link_libraries(sydCropImage ${SYD_STD_DB_LIBRARY})

# Create decay corrected image
# wrap_ggo(sydInsertDecayCorrectedImage_GGO_C
#   sydInsertDecayCorrectedImage.ggo
#   sydCommandLineWithTags.ggo
#   sydCommandLineDefault.ggo)
# add_executable(sydInsertDecayCorrectedImage sydInsertDecayCorrectedImage.cxx ${sydInsertDecayCorrectedImage_GGO_C})
# target_link_libraries(sydInsertDecayCorrectedImage ${SYD_STD_DB_LIBRARY})

# Create image with a different radionuclide
# wrap_ggo(sydInsertSubstituteRadionuclideImage_GGO_C
#   sydInsertSubstituteRadionuclideImage.ggo
#   sydCommandLineWithTags.ggo
#   sydCommandLineDefault.ggo)
# add_executable(sydInsertSubstituteRadionuclideImage
#   sydInsertSubstituteRadionuclideImage.cxx ${sydInsertSubstituteRadionuclideImage_GGO_C})
# target_link_libraries(sydInsertSubstituteRadionuclideImage ${SYD_STD_DB_LIBRARY})



# compute integrated activity in a ROI
# wrap_ggo(sydInsertFitResult_GGO_C
#   sydInsertFitResult.ggo
#   sydCommandLineWithFitOptions.ggo
#   sydCommandLineWithTags.ggo
#   sydCommandLineDefault.ggo)
# add_executable(sydInsertFitResult
#   sydInsertFitResult.cxx ${sydInsertFitResult_GGO_C})
# target_link_libraries(sydInsertFitResult ${SYD_STD_DB_LIBRARY})


# insert a timepoints from a roi / image
# wrap_ggo(sydInsertTimepoints_GGO_C sydInsertTimepoints.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
# add_executable(sydInsertTimepoints sydInsertTimepoints.cxx ${sydInsertTimepoints_GGO_C})
# target_link_libraries(sydInsertTimepoints ${SYD_STD_DB_LIBRARY})


# insert a timepoints from a pixel coord
# wrap_ggo(sydInsertPixelTimepoints_GGO_C sydInsertPixelTimepoints.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
# add_executable(sydInsertPixelTimepoints sydInsertPixelTimepoints.cxx ${sydInsertPixelTimepoints_GGO_C})
# target_link_libraries(sydInsertPixelTimepoints ${SYD_STD_DB_LIBRARY})


#----------------------------------------------------------
# External link Gate
# wrap_ggo(sydInsertGateOutput sydInsertGateOutput.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
# add_executable(sydInsertGateOutput sydInsertGateOutput.cxx sydInsertGateOutput_ggo.c)
# target_link_libraries(sydInsertGateOutput ${SYD_STD_DB_LIBRARY})

#----------------------------------------------------------
# Compute RootMeanSquareError in a mask
# wrap_ggo(sydInsertRootMeanSquareImage_GGO_C
#   sydInsertRootMeanSquareImage.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
# add_executable(sydInsertRootMeanSquareImage sydInsertRootMeanSquareImage.cxx ${sydInsertRootMeanSquareImage_GGO_C})
# target_link_libraries(sydInsertRootMeanSquareImage ${SYD_STD_DB_LIBRARY})


#----------------------------------------------------------
# Kmeans
# wrap_ggo(sydKmeansBuildInputData_GGO_C
#   sydKmeansBuildInputData.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
# add_executable(sydKmeansBuildInputData sydKmeansInputDataBuilder.cxx
#   sydKmeansBuildInputData.cxx ${sydKmeansBuildInputData_GGO_C})
# target_link_libraries(sydKmeansBuildInputData ${SYD_STD_DB_LIBRARY})

# wrap_ggo(sydKmeans_GGO_C
#   sydKmeans.ggo sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
# add_executable(sydKmeans sydKmeans.cxx ${sydKmeans_GGO_C})
# target_link_libraries(sydKmeans ${SYD_STD_DB_LIBRARY})
