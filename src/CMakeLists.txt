

#----------------------------------------------------------
# Old stuff to keep
add_subdirectory(${SYD_SOURCE_DIR}/src/old ${PROJECT_BINARY_DIR}/src/old)
#----------------------------------------------------------


#----------------------------------------------------------
# Syd Core
add_subdirectory(${SYD_SOURCE_DIR}/src/core ${PROJECT_BINARY_DIR}/src/core)
include_directories(${PROJECT_SOURCE_DIR}/src/core)
#----------------------------------------------------------


#----------------------------------------------------------
# DB Clinical Trial
add_subdirectory(${SYD_SOURCE_DIR}/src/db/clinicaltrial ${PROJECT_BINARY_DIR}/src/db/clinicaltrial)
include_directories(${PROJECT_BINARY_DIR}/src/db/clinicaltrial)
include_directories(${PROJECT_SOURCE_DIR}/src/db/clinicaltrial)
#----------------------------------------------------------


#----------------------------------------------------------
# DB Time Points
add_subdirectory(${SYD_SOURCE_DIR}/src/db/timepoints ${PROJECT_BINARY_DIR}/src/db/timepoints)
include_directories(${PROJECT_BINARY_DIR}/src/db/timepoints)
include_directories(${PROJECT_SOURCE_DIR}/src/db/timepoints)
#----------------------------------------------------------


#----------------------------------------------------------
set(SYD_LIBS
  ${DCMTK_LIBRARIES}
  sydTimePointsDatabase
  sydClinicalTrialDatabase
  sydCore
  sqlite3 odb-sqlite odb z
  ${CERES_LIBRARIES}
  ${ITK_LIBRARIES})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(syd syd.ggo)
add_executable(syd syd.cxx syd_ggo.c
  sydClearSeriesCommand.cxx
  sydDumpSeriesCommand.cxx
  sydCheckIntegrityCommand.cxx
  sydInsertTimePointCommand.cxx
  sydInsertDicomCommand.cxx)
target_link_libraries(syd ${SYD_LIBS})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydDump sydDump.ggo)
add_executable(sydDump sydDump.cxx sydDump_ggo.c sydDumpCommand.cxx)
target_link_libraries(sydDump ${SYD_LIBS})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydInsertDicom sydInsertDicom.ggo)
add_executable(sydInsertDicom sydInsertDicom.cxx sydInsertDicom_ggo.c sydInsertDicomCommand.cxx)
target_link_libraries(sydInsertDicom ${SYD_LIBS})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydInsertTimePoint sydInsertTimePoint.ggo)
add_executable(sydInsertTimePoint sydInsertTimePoint.cxx sydInsertTimePoint_ggo.c sydInsertTimePointCommand.cxx)
target_link_libraries(sydInsertTimePoint ${SYD_LIBS})
#----------------------------------------------------------
