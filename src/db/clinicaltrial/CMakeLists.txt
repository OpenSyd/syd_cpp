
#----------------------------------------------------------
wrap_odb(sydClinicalTrialDatabase Patient.hxx)
wrap_odb(sydClinicalTrialDatabase Study.hxx)
wrap_odb(sydClinicalTrialDatabase Serie.hxx)

add_library(sydClinicalTrialDatabase
  sydClinicalTrialDatabase.cxx
  Serie-odb.cxx
  Study-odb.cxx
  Patient-odb.cxx)
#----------------------------------------------------------
