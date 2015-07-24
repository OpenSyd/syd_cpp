
#----------------------------------------------------------
# Find dcmtk
find_package(DCMTK REQUIRED)
include_directories(${DCMTK_INCLUDE_DIRS})
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
