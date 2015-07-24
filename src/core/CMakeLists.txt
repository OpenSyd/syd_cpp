

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
#  sydDatabaseInformation.cxx
  sydImageUtils.cxx
#  sydPluginManager.cxx
  sydException.cxx
#  sydDatabase.cxx
#  sydDatabaseManager.cxx
  sydDicomCommon.cxx
  sydTimeActivityCurve.cxx
  sydPrintTable.cxx
)

target_link_libraries(sydCore
  ${DCMTK_LIBRARIES}
  ${Boost_LIBRARIES} ## needed on osx
  #  # sydCommonSchema
  #  # sydFit
  #   #odb-sqlite odb sqlite3 z
  #  # ${ITK_LIBRARIES}
  #   #${CERES_LIBRARIES}
)

#----------------------------------------------------------


#----------------------------------------------------------
# Fit library
add_library(sydFit
  sydFitModelBase.cxx
  sydFitModel_f1.cxx
  sydFitModel_f2.cxx
  sydFitModel_f3.cxx
  sydFitModel_f4.cxx
  sydFitModel_f4a.cxx
  sydFitModel_f4b.cxx
  sydFitModel_f4c.cxx
)

target_link_libraries(sydFit ${CERES_LIBRARIES})
#----------------------------------------------------------
