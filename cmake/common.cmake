#----------------------------------------------------------
macro(DD in)
    message(${in}=${${in}})
endmacro(DD)
#----------------------------------------------------------


#----------------------------------------------------------
#Set a reasonable build mode default if the user hasn't set any
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)
#----------------------------------------------------------


#----------------------------------------------------------
include_directories(${CLITK_SOURCE_DIR} ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
link_directories(${PROJECT_BINARY_DIR}/lib)
#For ggo odb includes from other directories
include_directories(${PROJECT_BINARY_DIR}/src)
#----------------------------------------------------------


#----------------------------------------------------------
# Building in the source tree is forbidden
if(PROJECT_BINARY_DIR STREQUAL ${PROJECT_SOURCE_DIR})
  message(FATAL_ERROR "Building in the source tree is not allowed ! Quit; remove the file 'CMakeCache.txt' and the folder 'CMakeFiles' and build outside the sources (for example 'mkdir build ; cmake <SYD_DIR>'.")
endif(PROJECT_BINARY_DIR STREQUAL ${PROJECT_SOURCE_DIR})
#----------------------------------------------------------


#----------------------------------------------------------
# Remove some MS Visual c++ flags
if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
endif(MSVC)
#----------------------------------------------------------
