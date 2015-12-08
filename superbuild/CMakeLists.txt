cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

# set a default build type if it is undefined, then make sure it goes in the cache
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Build configuration type" FORCE)
set(build_type ${CMAKE_BUILD_TYPE})

#=========================================================
macro(DD in)
    message(${in}=${${in}})
endmacro(DD)
#=========================================================

project(SYD)

include(ExternalProject)

set(base "${CMAKE_BINARY_DIR}")
set_property(DIRECTORY PROPERTY EP_BASE ${base})
set(source_prefix ${base}/Source)
set(build_prefix ${base}/Build)
set(install_prefix ${base}/Install)

find_package(Git REQUIRED)

#=========================================================
# ITK
ExternalProject_Add(ITK
  SOURCE_DIR ${source_prefix}/itk
  GIT_REPOSITORY git://itk.org/ITK.git
  GIT_TAG v4.8.1
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
    -DCMAKE_BUILD_TYPE:STRING=${build_type}
    -DBUILD_SHARED_LIBS:BOOL=OFF
    -DITK_DYNAMIC_LOADING:BOOL=OFF
    -DBUILD_EXAMPLES:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
)
set(ITK_DIR ${build_prefix}/ITK)
#=========================================================


#=========================================================
# ODB
ExternalProject_Add(ODB_compiler
  SOURCE_DIR ${source_prefix}/odb
  URL http://codesynthesis.com/download/odb/2.4/odb-2.4.0-i686-linux-gnu.tar.bz2
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  ## in SOURCE/odb
 )
ExternalProject_Add(ODB_lib
  SOURCE_DIR ${source_prefix}/odb_lib
  URL http://codesynthesis.com/download/odb/2.4/libodb-2.4.0.tar.bz2
  CONFIGURE_COMMAND ${source_prefix}/odb_lib/configure --prefix=${install_prefix}
 )
ExternalProject_Add(ODB_sqlite
  SOURCE_DIR ${source_prefix}/odb
  URL http://codesynthesis.com/download/odb/2.4/libodb-sqlite-2.4.0.tar.bz2
  CONFIGURE_COMMAND ${source_prefix}/odb_lib/configure --prefix=${install_prefix}
 )
#set(ODB_DIR ${build_prefix}/ODB)
#=========================================================



#=========================================================
# DCMTK

# Boost

# Ceres-solver

# var env

#=========================================================
# SYD
ExternalProject_Add(SYD
  DEPENDS ODB_compiler ODB_lib ODB_sqlite ITK
  SOURCE_DIR ${source_prefix}/syd
  GIT_REPOSITORY https://github.com/OpenSyd/syd.git
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
    -DCMAKE_BUILD_TYPE:STRING=${build_type}
    -DBUILD_SHARED_LIBS:BOOL=OFF
    -DBUILD_EXAMPLES:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF

)
set(ITK_DIR ${build_prefix}/ITK)
#=========================================================
