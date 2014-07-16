
#----------------------------------------------------------
# Find ITK (required)
find_package(ITK)
if(ITK_FOUND)
  include("${ITK_USE_FILE}")
else(ITK_FOUND)
  message(FATAL_ERROR "Cannot build without ITK.  Please set ITK_DIR.")
endif(ITK_FOUND)
#----------------------------------------------------------


#----------------------------------------------------------
# this is required (even if gdcm is inside itk)

# *********** FIXME : only needed on satyre ?

if(ITK_USE_SYSTEM_GDCM)
  message(STATUS "use gdcm")
  find_package(GDCM REQUIRED)
  include(${GDCM_USE_FILE})
endif()
#----------------------------------------------------------


#----------------------------------------------------------
# Find gengetopt, will create a target exe if not found
set(CMAKE_MODULE_PATH "${SYD_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
find_package(Gengetopt)
#----------------------------------------------------------
