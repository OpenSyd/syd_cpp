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
# itk
ExternalProject_Add(itk
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
  -DCMAKE_CXX_COMPILER:FILE=${CMAKE_CXX_COMPILER}
  )
set(itk_dir ${build_prefix}/itk)
#=========================================================


#=========================================================
# odb
ExternalProject_Add(odb_compiler
  SOURCE_DIR ${source_prefix}/odb_compiler
  #  URL http://codesynthesis.com/download/odb/2.4/odb-2.4.0-i686-linux-gnu.tar.bz2
  URL http://www.codesynthesis.com/download/odb/2.4/odb-2.4.0-x86_64-linux-gnu.tar.bz2
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  )
set(odb_exec ${source_prefix}/odb_compiler/bin/odb)

ExternalProject_Add(odb_lib
  SOURCE_DIR ${source_prefix}/odb_lib
  URL http://codesynthesis.com/download/odb/2.4/libodb-2.4.0.tar.bz2
  CONFIGURE_COMMAND ${source_prefix}/odb_lib/configure --prefix=${install_prefix}
  INSTALL_COMMAND make install
  )
set(odb_libodb_include_dirs ${install_prefix}/include)
set(odb_libodb_librarie ${install_prefix}/lib64/libodb.a)
set(odb_library_path ${install_prefix}/lib64)

ExternalProject_Add(odb_sqlite
  DEPENDS odb_lib
  SOURCE_DIR ${source_prefix}/odb_sqlite
  URL http://codesynthesis.com/download/odb/2.4/libodb-sqlite-2.4.0.tar.bz2
  CONFIGURE_COMMAND ${source_prefix}/odb_sqlite/configure --with-libodb=${odb_libodb_include_dirs} --prefix=${install_prefix}
  INSTALL_COMMAND make install
  )

set(odb_sqlite_include_dirs ${install_prefix}/include)
set(odb_sqlite_libraries ${install_prefix}/lib64)


#=========================================================



#=========================================================
# dcmtk
ExternalProject_Add(dcmtk
  SOURCE_DIR ${source_prefix}/dcmtk
  URL ftp://dicom.offis.de/pub/dicom/offis/software/dcmtk/dcmtk360/dcmtk-3.6.0.tar.gz
  #URL ftp://dicom.offis.de/pub/dicom/offis/software/dcmtk/dcmtk360/bin/dcmtk-3.6.0-linux-i686-static.tar.bz2
  GIT_REPOSITORY http://git.dcmtk.org/dcmtk.git
  GIT_TAG DCMTK-3.6.1_20140617
  CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
  -DCMAKE_BUILD_TYPE:STRING=${build_type}
  -DBUILD_SHARED_LIBS:BOOL=OFF
  -DBUILD_APPS:BOOL=OFF
  -DDCMTK_WITH_DOXYGEN:BOOL=OFF
  -DCMAKE_CXX_COMPILER:FILE=${CMAKE_CXX_COMPILER}
  )

set(DCMTK_DIR ${source_prefix}/dcmtk)


#=========================================================
# Boost
ExternalProject_Add(boost
  URL http://downloads.sourceforge.net/project/boost/boost/1.59.0/boost_1_59_0.tar.bz2
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ${source_prefix}/boost/bootstrap.sh
                    --prefix=${install_prefix}
                    --with-libraries=date_time
                    --with-libraries=filesystem
  BUILD_COMMAND ${source_prefix}/boost/b2 --prefix=${install_prefix}
  INSTALL_COMMAND ${source_prefix}/boost/b2 install
  )

#=========================================================
# Ceres-solver
ExternalProject_Add(eigen
  # URL http://bitbucket.org/eigen/eigen/get/3.2.7.tar.bz2
  ## URL not possible because require ssl cmake (not in cluster)
  GIT_REPOSITORY https://github.com/RLovelett/eigen
  GIT_TAG 3.3-alpha1
  SOURCE_DIR ${source_prefix}/eigen
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory "${source_prefix}/eigen/Eigen" "${install_prefix}/eigen/Eigen" && ${CMAKE_COMMAND} -E copy_directory "${source_prefix}/eigen/unsupported" "${install_prefix}/eigen/unsupported"
  )

set(eigen_include_dir ${install_prefix}/eigen)

ExternalProject_Add(glog
  SOURCE_DIR ${source_prefix}/glog
  #URL https://google-glog.googlecode.com/files/glog-0.3.3.tar.gz
  URL http://www.creatis.insa-lyon.fr/~dsarrut/glog-0.3.3.tar.gz
  CONFIGURE_COMMAND ${source_prefix}/glog/configure --prefix=${install_prefix}
  INSTALL_COMMAND make install
)
set(glog_library ${install_prefix}/lib/libglog.a)
set(glog_include_dir ${install_prefix}/include)

ExternalProject_Add(ceres
  DEPENDS eigen glog
  SOURCE_DIR ${source_prefix}/ceres-solver
  GIT_REPOSITORY https://ceres-solver.googlesource.com/ceres-solver
  CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
  -DCMAKE_BUILD_TYPE:STRING=${build_type}
  -DBUILD_SHARED_LIBS:BOOL=OFF
  -DBUILD_DOCUMENTATION:BOOL=OFF
  -DBUILD_EXAMPLES:BOOL=OFF
  -DBUILD_TESTING:BOOL=OFF
  -DEIGEN_INCLUDE_DIR:PATH=${eigen_include_dir}
  -DGLOG_LIBRARY:FILE=${glog_library}
  -DGLOG_INCLUDE_DIR:PATH=${glog_include_dir}
  -DCXX11:BOOL=ON
  -DCMAKE_CXX_COMPILER:FILE=${CMAKE_CXX_COMPILER}
  )

# var env

#=========================================================
# SYD

ExternalProject_Add(syd
  DEPENDS ceres odb_compiler odb_lib odb_sqlite dcmtk boost itk
  SOURCE_DIR ${source_prefix}/syd
  GIT_REPOSITORY https://github.com/OpenSyd/syd.git
  CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
  -DCMAKE_BUILD_TYPE:STRING=${build_type}
  -DBUILD_SHARED_LIBS:BOOL=OFF
  -DDCMTK_DIR:PATH=${DCMTK_DIR}
  -DCeres_DIR:PATH=${ceres_dir}
  -DITK_DIR:PATH=${itk_dir}
  -DODB_EXECUTABLE:FILE=${odb_exec}
  -DODB_LIBODB_INCLUDE_DIRS:PATH=${odb_libodb_include_dirs}
  -DODB_LIBODB_LIBRARIES:FILE=${odb_libodb_librarie}
  -DODB_LIBRARY_PATH:PATH=${odb_library_path}
  -DODB_SQLITE_INCLUDE_DIRS:PATH=${odb_sqlite_include_dirs}
  -DODB_SQLITE_LIBRARIES:PATH=${odb_sqlite_libraries}
  -DBoost_DIR=PATH=${install_prefix}
  )

#=========================================================
