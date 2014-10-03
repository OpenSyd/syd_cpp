

#----------------------------------------------------------
wrap_odb(sydClinicDatabase Patient.hxx)
wrap_odb(sydClinicDatabase Serie.hxx)
wrap_odb(sydClinicDatabase RoiType.hxx)

add_library(sydClinicDatabase
  sydClinicDatabase.cxx
  Serie-odb.cxx
  Patient-odb.cxx
  RoiType-odb.cxx)
#----------------------------------------------------------


#----------------------------------------------------------
wrap_odb(sydStudyDatabase Timepoint.hxx)
wrap_odb(sydStudyDatabase RawImage.hxx)
wrap_odb(sydStudyDatabase RoiMaskImage.hxx)

add_library(sydStudyDatabase
  sydStudyDatabase.cxx
  Timepoint-odb.cxx
  RawImage-odb.cxx
  RoiMaskImage-odb.cxx)
#----------------------------------------------------------
