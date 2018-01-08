
#----------------------------------------------------------
# Standard database : StandardDatabase
set(SCHEMA_NAME StandardDatabase)

wrap_odb(sydStandardDatabase sydPatient.h)
wrap_odb(sydStandardDatabase sydRadionuclide.h)
wrap_odb(sydStandardDatabase sydInjection.h)
wrap_odb(sydStandardDatabase sydDicomBase.h)
wrap_odb(sydStandardDatabase sydDicomSerie.h)
wrap_odb(sydStandardDatabase sydDicomStruct.h)
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
wrap_odb(sydStandardDatabase sydICRPOrganDose.h)

set(SYD_STANDARD_DATABASE_SRC

  sydStandardDatabase.cxx

  sydRadionuclide-odb.cxx sydRadionuclide-schema.cxx sydRadionuclide.cxx
  sydPatient-odb.cxx sydPatient-schema.cxx  sydPatient.cxx
  sydInjection-odb.cxx sydInjection-schema.cxx  sydInjection.cxx
  sydDicomBase-odb.cxx sydDicomBase-schema.cxx  sydDicomBase.cxx
  sydDicomSerie-odb.cxx sydDicomSerie-schema.cxx  sydDicomSerie.cxx
  sydDicomStruct-odb.cxx sydDicomStruct-schema.cxx  sydDicomStruct.cxx
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
  sydICRPOrganDose-odb.cxx sydICRPOrganDose-schema.cxx sydICRPOrganDose.cxx

  sydPatientTraits.cxx
  sydInjectionTraits.cxx
  sydRadionuclideTraits.cxx
  sydPixelUnitTraits.cxx
  sydDicomFileTraits.cxx
  sydDicomBaseTraits.cxx
  sydDicomSerieTraits.cxx
  sydDicomStructTraits.cxx
  sydImageTraits.cxx
  sydRoiMaskImageTraits.cxx
  sydRoiTypeTraits.cxx
  sydRoiStatisticTraits.cxx
  sydFitImagesTraits.cxx
  sydTimepointsTraits.cxx
  sydRoiTimepointsTraits.cxx
  sydFitTimepointsTraits.cxx
  sydElastixTraits.cxx
  sydICRPOrganDoseTraits.cxx

  sydPixelUnitHelper.cxx
  sydDicomBuilder.cxx
  sydDicomSerieHelper.cxx
  sydImageHelper.cxx
  sydFAFHelper.cxx
  sydInjectionHelper.cxx
  sydRoiMaskImageHelper.cxx
  sydUpdateRadionuclideFilter.cxx
  sydElastixHelper.cxx
  sydRadionuclideHelper.cxx
  sydRoiStatisticHelper.cxx
  sydTimepointsHelper.cxx
  sydFitImagesHelper.cxx
  sydGateHelper.cxx
  sydDicomStructHelper.cxx
  sydICRPOrganDoseHelper.cxx

  sydImageType.cxx
  sydThumbnail.cxx
  sydTimeIntegratedActivityImageBuilder.cxx

  sydICRP_RadiationData.cxx
  sydICRP_SpecificAbsorbedFraction.cxx
  sydSCoefficientCalculator.cxx
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
wrap_ggo(sydTest_GGO_C sydTest.ggo ../common_db/sydCommandLineWithTags.ggo sydCommandLineDefault.ggo)
add_executable(sydTest sydTest.cxx ${sydTest_GGO_C})
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
  ../common_db/sydCommandLineWithTags.ggo
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
  ../common_db/sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo )
add_executable(sydInsertImageFromDicom
  sydInsertImageFromDicom.cxx
  ${sydInsertImageFromDicom_GGO_C})
target_link_libraries(sydInsertImageFromDicom ${SYD_STD_DB_LIBRARY})

# identify the type of an image
wrap_ggo(sydIdentifyImageType_GGO_C
  sydIdentifyImageType.ggo
  sydCommandLineDefault.ggo )
add_executable(sydIdentifyImageType
  sydIdentifyImageType.cxx
  ${sydIdentifyImageType_GGO_C})
target_link_libraries(sydIdentifyImageType ${SYD_STD_DB_LIBRARY})

# Update image
wrap_ggo(sydUpdateImage_GGO_C
  sydUpdateImage.ggo
  ../common_db/sydCommandLineWithTags.ggo
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
  ../common_db/sydCommandLineWithTags.ggo
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
  ../common_db/sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo )
add_executable(sydInsertPlanarGeometricalMean
  sydInsertPlanarGeometricalMean.cxx
  ${sydInsertPlanarGeometricalMean_GGO_C})
target_link_libraries(sydInsertPlanarGeometricalMean ${SYD_STD_DB_LIBRARY})

# Create the projection of an image along a dimension
wrap_ggo(sydInsertProjectionImage_GGO_C
  sydInsertProjectionImage.ggo
  ../common_db/sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo )
add_executable(sydInsertProjectionImage
  sydInsertProjectionImage.cxx
  ${sydInsertProjectionImage_GGO_C})
target_link_libraries(sydInsertProjectionImage ${SYD_STD_DB_LIBRARY})

# Create the attenuation map of an image
wrap_ggo(sydFAF_ACF_Image_GGO_C
  sydFAF_ACF_Image.ggo
  ../common_db/sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo )
add_executable(sydFAF_ACF_Image
  sydFAF_ACF_Image.cxx
  ${sydFAF_ACF_Image_GGO_C})
target_link_libraries(sydFAF_ACF_Image ${SYD_STD_DB_LIBRARY})

# Create the registered attenuation map of an image
wrap_ggo(sydFAF_RegisterPlanarImage_GGO_C
  sydFAF_RegisterPlanarImage.ggo
  ../common_db/sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo )
add_executable(sydFAF_RegisterPlanarImage
  sydFAF_RegisterPlanarImage.cxx
  ${sydFAF_RegisterPlanarImage_GGO_C})
target_link_libraries(sydFAF_RegisterPlanarImage ${SYD_STD_DB_LIBRARY})

# Create the attenuation corrected of a planar image
wrap_ggo(sydFAF_ACGM_Image_GGO_C
  sydFAF_ACGM_Image.ggo
  ../common_db/sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo )
add_executable(sydFAF_ACGM_Image
  sydFAF_ACGM_Image.cxx
  ${sydFAF_ACGM_Image_GGO_C})
target_link_libraries(sydFAF_ACGM_Image ${SYD_STD_DB_LIBRARY})

# # Create the FAF mask
# wrap_ggo(sydInsertFAFMask_GGO_C
#   sydInsertFAFMask.ggo
#   ../common_db/sydCommandLineWithTags.ggo
#   sydCommandLineWithImageInfo.ggo
#   ../common_db/sydCommandLineWithComments.ggo
#   sydCommandLineDefault.ggo )
# add_executable(sydInsertFAFMask
#   sydInsertFAFMask.cxx
#   ${sydInsertFAFMask_GGO_C})
# target_link_libraries(sydInsertFAFMask ${SYD_STD_DB_LIBRARY})

# Create the FAF corrected of a SPECT image
wrap_ggo(sydFAF_Calibration_GGO_C
  sydFAF_Calibration.ggo
  ../common_db/sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo )
add_executable(sydFAF_Calibration
  sydFAF_Calibration.cxx
  ${sydFAF_Calibration_GGO_C})
target_link_libraries(sydFAF_Calibration ${SYD_STD_DB_LIBRARY})

# Create the manual registered image
wrap_ggo(sydInsertManualRegisteredImage_GGO_C
  sydInsertManualRegisteredImage.ggo
  ../common_db/sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo )
add_executable(sydInsertManualRegisteredImage
  sydInsertManualRegisteredImage.cxx
  ${sydInsertManualRegisteredImage_GGO_C})
target_link_libraries(sydInsertManualRegisteredImage ${SYD_STD_DB_LIBRARY})

# Create the flipped image
wrap_ggo(sydInsertFlippedImage_GGO_C
  sydInsertFlippedImage.ggo
  ../common_db/sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo )
add_executable(sydInsertFlippedImage
  sydInsertFlippedImage.cxx
  ${sydInsertFlippedImage_GGO_C})
target_link_libraries(sydInsertFlippedImage ${SYD_STD_DB_LIBRARY})

# Insert a new RoiMaskImage
wrap_ggo(sydInsertRoiMaskImage_GGO_C
  sydInsertRoiMaskImage.ggo
  ../common_db/sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertRoiMaskImage
  sydInsertRoiMaskImage.cxx
  ${sydInsertRoiMaskImage_GGO_C})
target_link_libraries(sydInsertRoiMaskImage ${SYD_STD_DB_LIBRARY})

# Insert a new RoiMaskImage from a struct
wrap_ggo(sydInsertRoiMaskImageFromDicomStruct_GGO_C
  sydInsertRoiMaskImageFromDicomStruct.ggo
  ../common_db/sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertRoiMaskImageFromDicomStruct
  sydInsertRoiMaskImageFromDicomStruct.cxx
  ${sydInsertRoiMaskImageFromDicomStruct_GGO_C})
target_link_libraries(sydInsertRoiMaskImageFromDicomStruct ${SYD_STD_DB_LIBRARY})

# Insert a new RoiStatistic
wrap_ggo(sydInsertRoiStatistic_GGO_C
  sydInsertRoiStatistic.ggo
  ../common_db/sydCommandLineWithTags.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertRoiStatistic
  sydInsertRoiStatistic.cxx
  ${sydInsertRoiStatistic_GGO_C})
target_link_libraries(sydInsertRoiStatistic ${SYD_STD_DB_LIBRARY})

# Create the Chang attenuation image
wrap_ggo(sydInsertChangAttenuationImage_GGO_C
  sydInsertChangAttenuationImage.ggo
  ../common_db/sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo )
add_executable(sydInsertChangAttenuationImage
  sydInsertChangAttenuationImage.cxx
  ${sydInsertChangAttenuationImage_GGO_C})
target_link_libraries(sydInsertChangAttenuationImage ${SYD_STD_DB_LIBRARY})

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
  ../common_db/sydCommandLineWithTags.ggo
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
  ../common_db/sydCommandLineWithTags.ggo
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
  ../common_db/sydCommandLineWithTags.ggo
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
  ../common_db/sydCommandLineWithTags.ggo
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
  ../common_db/sydCommandLineWithTags.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertRoiBasedTimeIntegratedActivity
  sydInsertRoiBasedTimeIntegratedActivity.cxx
  sydTimeIntegratedActivityImageBuilder.cxx
  ${sydInsertRoiBasedTimeIntegratedActivity_GGO_C})
target_link_libraries(sydInsertRoiBasedTimeIntegratedActivity ${SYD_STD_DB_LIBRARY})

# Create a RoiTimepoints from a list of RoiStatistics
wrap_ggo(sydInsertRoiTimepoints_GGO_C
  sydInsertRoiTimepoints.ggo
  ../common_db/sydCommandLineWithTags.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertRoiTimepoints
  sydInsertRoiTimepoints.cxx
  sydTimeIntegratedActivityImageBuilder.cxx
  ${sydInsertRoiTimepoints_GGO_C})
target_link_libraries(sydInsertRoiTimepoints ${SYD_STD_DB_LIBRARY})

wrap_ggo(sydComputeICRPSCoefficient_GGO_C
  sydComputeICRPSCoefficient.ggo
  sydCommandLineDefault.ggo)
add_executable(sydComputeICRPSCoefficient
  sydComputeICRPSCoefficient.cxx
  ${sydComputeICRPSCoefficient_GGO_C})
target_link_libraries(sydComputeICRPSCoefficient ${SYD_STD_DB_LIBRARY})

wrap_ggo(sydInsertTimeIntegratedActivityTimepoints_GGO_C
  sydInsertTimeIntegratedActivityTimepoints.ggo
  ../common_db/sydCommandLineWithTags.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineWithFitOptions.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertTimeIntegratedActivityTimepoints
  sydInsertTimeIntegratedActivityTimepoints.cxx
  ${sydInsertTimeIntegratedActivityTimepoints_GGO_C})
target_link_libraries(sydInsertTimeIntegratedActivityTimepoints ${SYD_STD_DB_LIBRARY})

wrap_ggo(sydInsertICRPOrganDose_GGO_C
  sydInsertICRPOrganDose.ggo
  ../common_db/sydCommandLineWithTags.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertICRPOrganDose
  sydInsertICRPOrganDose.cxx
  ${sydInsertICRPOrganDose_GGO_C})
target_link_libraries(sydInsertICRPOrganDose ${SYD_STD_DB_LIBRARY})

#----------------------------------------------------------
# External tool: clitkExtractPatient
wrap_ggo(syd_clitkExtractPatient_GGO_C
  syd_clitkExtractPatient.ggo
  ../common_db/sydCommandLineWithTags.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineWithImageInfo.ggo
  sydCommandLineDefault.ggo)
add_executable(syd_clitkExtractPatient
  syd_clitkExtractPatient.cxx ${syd_clitkExtractPatient_GGO_C})
target_link_libraries(syd_clitkExtractPatient ${SYD_STD_DB_LIBRARY})

wrap_ggo(sydInsertElastixRegistration_GGO_C
  sydInsertElastixRegistration.ggo
  ../common_db/sydCommandLineWithTags.ggo
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
  ../common_db/sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(syd_transformix
  syd_transformix.cxx
  ${syd_transformix_GGO_C})
target_link_libraries(syd_transformix ${SYD_STD_DB_LIBRARY})

wrap_ggo(syd_gate syd_gate.ggo sydCommandLineDefault.ggo)
add_executable(syd_gate syd_gate.cxx syd_gate_ggo.c)
target_link_libraries(syd_gate ${SYD_STD_DB_LIBRARY})

wrap_ggo(sydInsertGateOutput_GGO_C
  sydInsertGateOutput.ggo
  ../common_db/sydCommandLineWithTags.ggo
  sydCommandLineWithImageInfo.ggo
  ../common_db/sydCommandLineWithComments.ggo
  sydCommandLineDefault.ggo)
add_executable(sydInsertGateOutput sydInsertGateOutput.cxx ${sydInsertGateOutput_GGO_C})
target_link_libraries(sydInsertGateOutput ${SYD_STD_DB_LIBRARY})


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
wrap_ggo(sydInsertPixelTimepoints_GGO_C
  sydInsertPixelTimepoints.ggo
  sydCommandLineWithTags.ggo
  sydCommandLineDefault.ggo)
# add_executable(sydInsertPixelTimepoints
#   sydInsertPixelTimepoints.cxx
#   ${sydInsertPixelTimepoints_GGO_C})
# target_link_libraries(sydInsertPixelTimepoints ${SYD_STD_DB_LIBRARY})


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
