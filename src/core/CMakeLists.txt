

#----------------------------------------------------------
# pstream to allow reading from pip
include_directories(${SYD_SOURCE_DIR}/src/core/pstreams)

#----------------------------------------------------------
# Common library
add_library(sydCore
  md5.cpp
  sydCommon.cxx
  sydFileUtils.cxx
  sydLog.cxx
  sydException.cxx
  sydDicomCommon.cxx
  sydPrintTable.cxx
)

target_link_libraries(sydCore
  ${DCMTK_LIBRARIES}
  ${Boost_LIBRARIES} ## needed on osx
  )

# wrap_ggo(sydOdbXmlParser_GGO_C sydOdbXmlParser.ggo )
# add_executable(sydOdbXmlParser sydOdbXmlParser.cxx ${sydOdbXmlParser_GGO_C})
# target_link_libraries(sydOdbXmlParser sydCore)

#----------------------------------------------------------


#----------------------------------------------------------
# ITK tools library
add_library(sydItkUtils
  sydImageUtils.cxx
)

target_link_libraries(sydItkUtils ${ITK_LIBRARIES})
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
  sydIntegratedActivityImageBuilder.cxx
)

target_link_libraries(sydFit ${CERES_LIBRARIES})
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

  sydIntegratedActivityImageBuilder.h
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
#DD(SYD_INCLUDE_FILES)
