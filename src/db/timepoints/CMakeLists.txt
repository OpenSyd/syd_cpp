
#----------------------------------------------------------
wrap_odb(sydTimePointsDatabase TimePoint.hxx)

add_library(sydTimePointsDatabase
  sydTimePointsDatabase.cxx
  TimePoint-odb.cxx)
#----------------------------------------------------------
