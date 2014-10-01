

#----------------------------------------------------------
wrap_odb(sydClinicDatabase Patient.hxx)
wrap_odb(sydClinicDatabase Serie.hxx)

add_library(sydClinicDatabase
  sydClinicDatabase.cxx
  Serie-odb.cxx
  Patient-odb.cxx)
#----------------------------------------------------------


# #----------------------------------------------------------
# wrap_odb(sydTimepointsDatabase Timepoint.hxx)

# add_library(sydTimepointsDatabase
#   sydTimepointsDatabase.cxx
#   Timepoint-odb.cxx)
# #----------------------------------------------------------


#----------------------------------------------------------
wrap_odb(sydStudyDatabase Timepoint.hxx)
wrap_odb(sydStudyDatabase RawImage.hxx)

add_library(sydStudyDatabase
  sydStudyDatabase.cxx
  Timepoint-odb.cxx
  RawImage-odb.cxx)
#----------------------------------------------------------
