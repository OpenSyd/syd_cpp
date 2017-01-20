

#----------------------------------------------------------
# pstream to allow reading from pip
include_directories(${SYD_SOURCE_DIR}/src/core/pstreams)

#----------------------------------------------------------
# Common library
add_library(sydCore SHARED
  md5.cpp
  sydCommon.cxx
  sydFileUtils.cxx
  sydLog.cxx
  sydException.cxx
  sydHistogram.cxx
  sydNDimPoints.cxx
  kmeans/KMeans.cpp
  kmeans/KmTree.cpp
  kmeans/KmUtils.cpp
  )

if (APPLE) ## needed on osx
  target_link_libraries(sydCore ${Boost_LIBRARIES})
endif(APPLE)

#----------------------------------------------------------
# ITK tools library
add_library(sydItkUtils SHARED
  sydImageUtils.cxx
  sydDicomUtils.cxx
  sydKmeansFilter.cxx
  )

target_link_libraries(sydItkUtils sydCore ${ITK_LIBRARIES})
#----------------------------------------------------------


#----------------------------------------------------------
# Fit tools library
add_library(sydFit
  sydTimeActivityCurve.cxx
  sydFitModelBase.cxx
  sydFitModel_f1.cxx
  sydFitModel_f2.cxx
  sydFitModel_f3.cxx
  sydFitModel_f4.cxx
  sydFitModel_f4a.cxx
  sydFitModel_f4b.cxx
  sydFitModel_f4c.cxx
  sydFitOutputImage.cxx
  sydTimeIntegratedActivityFilter.cxx
  sydTimeIntegratedActivityImageFilter.cxx
  sydTimeIntegratedActivityFitOptions.cxx
  )

target_link_libraries(sydFit ${CERES_LIBRARIES} glog)
#----------------------------------------------------------


#----------------------------------------------------------
# prepare include
set(SYD_INCLUDE_FILES ${SYD_INCLUDE_FILES}
  md5.h
  sydLog.h
  sydLogMacro.h
  sydCommon.h
  sydCommonGengetopt.h
  sydDD.h
  sydException.h

  sydTimeIntegratedActivityImageFilter.h
  sydTimeIntegratedActivityFitOptions.h
  itkMedianWithMaskImageFilter.h
  sydImageCrop.h
  sydImageFillHoles.h
  sydImageFlip.h
  sydImageUtils.h
  sydImage_GaussianFilter.h

  sydTestUtils.h
  sydFileUtils.h
  sydPrintTable.h
  sydTimeActivityCurve.h
  sydDicomCommon.h

  sydFitModel_f4c.h
  sydFitModelBase.h
  sydFitModels.h
  sydFitOutputImage.h
  sydFitModel_f1.h
  sydFitModel_f2.h
  sydFitModel_f3.h
  sydFitModel_f4.h
  sydFitModel_f4a.h
  sydFitModel_f4b.h
  PARENT_SCOPE
  )
