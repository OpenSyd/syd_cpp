
#----------------------------------------------------------
wrap_odb(sydClinicDatabase Patient.hxx)
wrap_odb(sydClinicDatabase Serie.hxx)

add_library(sydClinicDatabase
  sydClinicDatabase.cxx
  Serie-odb.cxx
  Patient-odb.cxx)
#----------------------------------------------------------


#----------------------------------------------------------
wrap_odb(sydTimePointsDatabase TimePoint.hxx)

add_library(sydTimePointsDatabase
  sydTimePointsDatabase.cxx
  TimePoint-odb.cxx)
#----------------------------------------------------------
