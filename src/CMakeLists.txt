

#----------------------------------------------------------
## This is required on some system (linux) to allow linking
if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC"  )
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC"  )
endif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
#----------------------------------------------------------


#----------------------------------------------------------
# Policy for shared library on OSX
cmake_policy(SET CMP0042 NEW) # CMake 3.0
#----------------------------------------------------------


#----------------------------------------------------------
# Syd libCore : main library
include_directories(${PROJECT_SOURCE_DIR}/src/core)
add_subdirectory(${SYD_SOURCE_DIR}/src/core)
#----------------------------------------------------------


#----------------------------------------------------------
# Syd standard DB
add_subdirectory(${SYD_SOURCE_DIR}/src/std_db)

#----------------------------------------------------------


#----------------------------------------------------------
# Syd standard tools
# include_directories(${PROJECT_BINARY_DIR}/src/tools)
# include_directories(${PROJECT_SOURCE_DIR}/src/tools)
add_subdirectory(${SYD_SOURCE_DIR}/src/tools)
# ${PROJECT_BINARY_DIR}/src/tools)
#----------------------------------------------------------


#----------------------------------------------------------
# wrap_ggo(sydDump sydDump.ggo)
# add_executable(sydDump sydDump.cxx sydDump_ggo.c)
# target_link_libraries(sydDump sydCore ${SYD_LIBS})
#----------------------------------------------------------


#----------------------------------------------------------
# wrap_ggo(sydCreateEmptyClinicDatabase sydCreateEmptyClinicDatabase.ggo)
# add_executable(sydCreateEmptyClinicDatabase
#   sydCreateEmptyClinicDatabase.cxx sydCreateEmptyClinicDatabase_ggo.c)
# target_link_libraries(sydCreateEmptyClinicDatabase ${SYD_LIBS})
#----------------------------------------------------------


#----------------------------------------------------------
# wrap_ggo(sydDelete sydDelete.ggo)
# add_executable(sydDelete sydDelete.cxx sydDelete_ggo.c)
# target_link_libraries(sydDelete ${SYD_LIBS})
#----------------------------------------------------------


#----------------------------------------------------------
# wrap_ggo(sydInsertInjection sydInsertInjection.ggo)
# add_executable(sydInsertInjection sydInsertInjection.cxx sydInsertInjection_ggo.c)
# target_link_libraries(sydInsertInjection ${SYD_LIBS})
#----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydInsertDicom sydInsertDicom.ggo)
# add_executable(sydInsertDicom sydInsertDicom.cxx sydInsertDicom_ggo.c)
# target_link_libraries(sydInsertDicom ${SYD_LIBS})
# #----------------------------------------------------------



#----------------------------------------------------------
#wrap_ggo(sydDelete sydDelete.ggo)
#add_subdirectory(${SYD_SOURCE_DIR}/src/test-plugin)
# add_executable(test-plugin test-plugin.cxx )
# target_link_libraries(test-plugin ${SYD_LIBS})
#----------------------------------------------------------



# #----------------------------------------------------------
# wrap_ggo(sydUpdate sydUpdate.ggo)
# add_executable(sydUpdate sydUpdate.cxx sydUpdate_ggo.c)
# target_link_libraries(sydUpdate ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydInsertAverageCT sydInsertAverageCT.ggo)
# add_executable(sydInsertAverageCT sydInsertAverageCT.cxx
#   sydInsertAverageCTCommand.cxx sydInsertAverageCT_ggo.c)
# target_link_libraries(sydInsertAverageCT ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydCreateDatabase sydCreateDatabase.ggo)
# add_executable(sydCreateDatabase sydCreateDatabase.cxx sydCreateDatabase_ggo.c)
# target_link_libraries(sydCreateDatabase ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydInsertTimepoint sydInsertTimepoint.ggo)
# add_executable(sydInsertTimepoint sydInsertTimepoint.cxx sydInsertTimepoint_ggo.c sydInsertTimepointCommand.cxx)
# target_link_libraries(sydInsertTimepoint ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydActivity sydActivity.ggo)
# add_executable(sydActivity sydActivity.cxx sydActivity_ggo.c
#   sydActivityCommandBase.cxx
#   sydActivityPeakCommand.cxx
#   sydActivityLambdaCommand.cxx
#   sydActivityTimeIntegratedCommand.cxx
#   sydActivityCountCommand.cxx)
# target_link_libraries(sydActivity ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydCheckIntegrity sydCheckIntegrity.ggo)
# add_executable(sydCheckIntegrity sydCheckIntegrity.cxx sydCheckIntegrity_ggo.c)
# target_link_libraries(sydCheckIntegrity ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydRegister sydRegister.ggo)
# add_executable(sydRegister sydRegister.cxx sydRegister_ggo.c sydRegisterCommand.cxx)
# target_link_libraries(sydRegister ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydCropCT sydCropCT.ggo)
# add_executable(sydCropCT sydCropCT.cxx sydCropCT_ggo.c sydCropCTCommand.cxx)
# target_link_libraries(sydCropCT ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydInsertRoi sydInsertRoi.ggo)
# add_executable(sydInsertRoi sydInsertRoi.cxx sydInsertRoi_ggo.c sydInsertRoiCommand.cxx)
# target_link_libraries(sydInsertRoi ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydStitchSpect sydStitchSpect.ggo)
# add_executable(sydStitchSpect sydStitchSpect.cxx sydStitchSpect_ggo.c sydStitchSpectCommand.cxx)
# target_link_libraries(sydStitchSpect ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydConvertDicomSerie sydConvertDicomSerie.ggo)
# add_executable(sydConvertDicomSerie sydConvertDicomSerie.cxx sydConvertDicomSerie_ggo.c)
# target_link_libraries(sydConvertDicomSerie ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydUpdateSpect sydUpdateSpect.ggo)
# add_executable(sydUpdateSpect sydUpdateSpect.cxx sydUpdateSpect_ggo.c)
# target_link_libraries(sydUpdateSpect ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydCreateTempSpectImage sydCreateTempSpectImage.ggo)
# add_executable(sydCreateTempSpectImage sydCreateTempSpectImage.cxx sydCreateTempSpectImage_ggo.c)
# target_link_libraries(sydCreateTempSpectImage ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydCreateTimeIntegratedSpectImage sydCreateTimeIntegratedSpectImage.ggo)
# add_executable(sydCreateTimeIntegratedSpectImage
#   sydCreateTimeIntegratedSpectImage.cxx
#   sydCreateTimeIntegratedSpectImage_ggo.c)
# target_link_libraries(sydCreateTimeIntegratedSpectImage ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydTestSpectImage sydTestSpectImage.ggo)
# add_executable(sydTestSpectImage
#   sydTestSpectImage.cxx
#   sydTestSpectImage_ggo.c)
# target_link_libraries(sydTestSpectImage ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydTestCalibration sydTestCalibration.ggo)
# add_executable(sydTestCalibration
#   sydTestCalibration.cxx
#   sydTestCalibration_ggo.c)
# target_link_libraries(sydTestCalibration ${SYD_LIBS})
# #----------------------------------------------------------


# #----------------------------------------------------------
# wrap_ggo(sydSetCalibrationFactor sydSetCalibrationFactor.ggo)
# add_executable(sydSetCalibrationFactor
#   sydSetCalibrationFactor.cxx
#   sydSetCalibrationFactor_ggo.c)
# target_link_libraries(sydSetCalibrationFactor ${SYD_LIBS})
# #----------------------------------------------------------
