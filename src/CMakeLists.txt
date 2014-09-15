

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
find_package(DCMTK REQUIRED)
include_directories(${DCMTK_INCLUDE_DIRS})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(syd syd.ggo)
add_executable(syd syd.cxx syd_ggo.c
  sydClearSeriesCommand.cxx
  sydAddTimePointCommand.cxx
  sydInsertDicomCommand.cxx)
target_link_libraries(syd ${DCMTK_LIBRARIES}
  sydTimePointsDatabase
  sydClinicalTrialDatabase
  sydCore
  sqlite3 odb-sqlite odb  z ${CERES_LIBRARIES})
# ${ITK_LIBRARIES}
#----------------------------------------------------------
