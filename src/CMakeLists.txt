

#----------------------------------------------------------
wrap_odb(sydLib sydPatient.hxx)
wrap_odb(sydLib sydStudy.hxx)
wrap_odb(sydLib sydSerie.hxx)
wrap_odb(sydLib sydRoiStudy.hxx)
wrap_odb(sydLib sydRoiType.hxx)
wrap_odb(sydLib sydRoiSerie.hxx)
wrap_odb(sydLib sydProperty.hxx)
add_library(sydLib sydCommon.cxx sydEI.cxx sydTimeActivityCurve.cxx
  sydPatient-odb.cxx sydStudy-odb.cxx sydRoiStudy-odb.cxx sydProperty-odb.cxx
  sydSerie-odb.cxx sydRoiType-odb.cxx sydRoiSerie-odb.cxx)
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydTest_GGO_C sydTest.ggo)
add_executable(sydTest sydTest.cxx sydTest_ggo.c)
target_link_libraries(sydTest ${ITK_LIBRARIES} sydLib sqlite3 odb-sqlite odb)
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(syd syd.ggo)
add_executable(syd syd.cxx syd_ggo.c sydFit.cxx sydQuery.cxx sydCompute.cxx
  syd_Time_Integrated_Activity.cxx
  syd_ROI_Peak_Command.cxx
  syd_ROI_Time_Integrated_Activity_Command.cxx sydCommand.cxx syd_With_Verbose_Flag.cxx )
target_link_libraries(syd ${ITK_LIBRARIES} sydLib sqlite3 odb-sqlite odb clitkCommon ${CERES_LIBRARIES})
#----------------------------------------------------------


#----------------------------------------------------------
find_package(Boost REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
else(Boost_FOUND)
  message(FATAL_ERROR "Cannot build without Boost.  Please set Boost_DIR.")
endif(Boost_FOUND)

add_executable(sydTestCmdLine sydTestCmdLine.cxx)
target_link_libraries(sydTestCmdLine ${ITK_LIBRARIES} sydLib sqlite3 odb-sqlite odb -lboost_program_options)
#----------------------------------------------------------


#----------------------------------------------------------
add_library(sydTestLibExplicInst sydTestLibExplicInst.cxx)

add_executable(sydTestExplicInst sydTestExplicInst.cxx)
target_link_libraries(sydTestExplicInst sydTestLibExplicInst ${ITK_LIBRARIES} clitkCommon)

#----------------------------------------------------------
