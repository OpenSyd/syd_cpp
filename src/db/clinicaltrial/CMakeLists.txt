
#----------------------------------------------------------
wrap_odb(sydClinicalTrialDatabase Patient.hxx)
wrap_odb(sydClinicalTrialDatabase Serie.hxx)

add_library(sydClinicalTrialDatabase
  sydClinicalTrialDatabase.cxx
  Serie-odb.cxx
  Patient-odb.cxx)
#----------------------------------------------------------
