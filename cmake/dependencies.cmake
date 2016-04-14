
#----------------------------------------------------------
# Find dcmtk
find_package(DCMTK REQUIRED)
include_directories(${DCMTK_INCLUDE_DIRS})

# Later, in odb, we need the base include path, so we compute it here
list(GET DCMTK_INCLUDE_DIRS 0 DCMTK_INCLUDE_DIR_BASE)
set(DCMTK_INCLUDE_DIR_BASE ${DCMTK_INCLUDE_DIR_BASE}/../..)

#----------------------------------------------------------


#----------------------------------------------------------
# Find Odb
find_package(ODB REQUIRED OPTIONAL_COMPONENTS sqlite)
include(${ODB_USE_FILE})
#----------------------------------------------------------


#----------------------------------------------------------
# Find ceres
find_package(Ceres REQUIRED)
include_directories(${CERES_INCLUDE_DIRS})
#----------------------------------------------------------


#----------------------------------------------------------
# Find Gengetopt
find_package(Gengetopt)
#----------------------------------------------------------


#----------------------------------------------------------
# optional plot python
OPTION(SYD_PLOT_MODULE "Plot module (require Python)." OFF)
IF(SYD_PLOT_MODULE)
  find_package(PythonLibs REQUIRED)
  include_directories(${PYTHON_INCLUDE_DIR})
ENDIF(SYD_PLOT_MODULE)
#----------------------------------------------------------