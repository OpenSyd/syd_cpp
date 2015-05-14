
#----------------------------------------------------------
# Find ITK (required)
find_package(ITK REQUIRED)
if(NOT ITK_FOUND)
  message(FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
endif(NOT ITK_FOUND)
include(${ITK_USE_FILE})
#----------------------------------------------------------


#----------------------------------------------------------
# Find gengetopt, will create a target exe if not found
set(CMAKE_MODULE_PATH "${SYD_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
find_package(Gengetopt)
#----------------------------------------------------------


#----------------------------------------------------------
# Find dcmtk
find_package(DCMTK REQUIRED)
include_directories(${DCMTK_INCLUDE_DIRS})
#----------------------------------------------------------
