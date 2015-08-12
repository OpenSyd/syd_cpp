

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
  sydIntegratedActivityImageBuilder.cxx
)

target_link_libraries(sydFit ${CERES_LIBRARIES})
#----------------------------------------------------------
