

#----------------------------------------------------------
wrap_odb(sydLib sydPatient.hxx)
wrap_odb(sydLib sydStudy.hxx)
wrap_odb(sydLib sydSerie.hxx)
wrap_odb(sydLib sydRoiStudy.hxx)
wrap_odb(sydLib sydRoiType.hxx)
wrap_odb(sydLib sydRoiSerie.hxx)
add_library(sydLib sydCommon.cxx sydQuery.cxx
  sydPatient-odb.cxx sydStudy-odb.cxx sydRoiStudy-odb.cxx
  sydSerie-odb.cxx sydRoiType-odb.cxx sydRoiSerie-odb.cxx)
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydTest_GGO_C sydTest.ggo)
add_executable(sydTest sydTest.cxx sydTest_ggo.c)
#message(status ${ITK_LIBRARIES})
target_link_libraries(sydTest ${ITK_LIBRARIES} sydLib sqlite3 odb-sqlite odb)
#----------------------------------------------------------


#set(ODB_FILES sydPatient.hxx sydStudy.hxx)


wrap_ggo(syd syd.ggo)
add_executable(syd syd.cxx syd_ggo.c)
target_link_libraries(syd ${ITK_LIBRARIES} sydLib sqlite3 odb-sqlite odb clitkCommon)
