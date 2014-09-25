

#----------------------------------------------------------
# Old stuff to keep
#add_subdirectory(${SYD_SOURCE_DIR}/src/old ${PROJECT_BINARY_DIR}/src/old)
#----------------------------------------------------------


#----------------------------------------------------------
# Syd Core
add_subdirectory(${SYD_SOURCE_DIR}/src/core ${PROJECT_BINARY_DIR}/src/core)
include_directories(${PROJECT_SOURCE_DIR}/src/core)
#----------------------------------------------------------


#----------------------------------------------------------
# DB
add_subdirectory(${SYD_SOURCE_DIR}/src/db ${PROJECT_BINARY_DIR}/src/db)
include_directories(${PROJECT_BINARY_DIR}/src/db)
include_directories(${PROJECT_SOURCE_DIR}/src/db)
#----------------------------------------------------------


#----------------------------------------------------------
set(SYD_LIBS
  ${DCMTK_LIBRARIES}
  sydClinicDatabase
  sydTimepointsDatabase
  sydCore
  sqlite3 odb-sqlite odb z
  ${CERES_LIBRARIES}
  ${ITK_LIBRARIES})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydDumpClinic sydDumpClinic.ggo)
add_executable(sydDumpClinic sydDumpClinic.cxx sydDumpClinic_ggo.c sydDumpClinicCommand.cxx)
target_link_libraries(sydDumpClinic ${SYD_LIBS})

wrap_ggo(sydDumpStudy sydDumpStudy.ggo)
add_executable(sydDumpStudy sydDumpStudy.cxx sydDumpStudy_ggo.c sydDumpStudyCommand.cxx)
target_link_libraries(sydDumpStudy ${SYD_LIBS})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydInsertDicom sydInsertDicom.ggo)
add_executable(sydInsertDicom sydInsertDicom.cxx sydInsertDicom_ggo.c sydInsertDicomCommand.cxx)
target_link_libraries(sydInsertDicom ${SYD_LIBS})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydInsertTimepoint sydInsertTimepoint.ggo)
add_executable(sydInsertTimepoint sydInsertTimepoint.cxx sydInsertTimepoint_ggo.c sydInsertTimepointCommand.cxx)
target_link_libraries(sydInsertTimepoint ${SYD_LIBS})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydCheckIntegrity sydCheckIntegrity.ggo)
add_executable(sydCheckIntegrity sydCheckIntegrity.cxx sydCheckIntegrity_ggo.c sydCheckIntegrityCommand.cxx)
target_link_libraries(sydCheckIntegrity ${SYD_LIBS})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydRegister sydRegister.ggo)
add_executable(sydRegister sydRegister.cxx sydRegister_ggo.c sydRegisterCommand.cxx)
target_link_libraries(sydRegister ${SYD_LIBS})
#----------------------------------------------------------
