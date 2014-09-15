/*=========================================================================
  Program:   syd

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ===========================================================================**/

// syd
#include "sydAddTimePointCommand.h"

// --------------------------------------------------------------------
syd::AddTimePointCommand::AddTimePointCommand():DatabaseCommand()
{
  db_ = NULL;
  tpdb_ = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::AddTimePointCommand::~AddTimePointCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::AddTimePointCommand::AddDatabase(syd::Database * d)
{
  if (databases_.size() == 0) { // first db
    DatabaseCommand::AddDatabase(d);
    tpdb_ = static_cast<TimePointsDatabase*>(d);
  }
  else {
    if (databases_.size() == 1) { // second db
    DatabaseCommand::AddDatabase(d);
    db_ = static_cast<ClinicalTrialDatabase*>(d);
    }
    else {
      LOG(FATAL) << "TimePointsDatabase::AddDatabase error. First provide TimePointsDatabase then ClinicalTrialDatabase.";
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::AddTimePointCommand::SetArgs(char ** inputs, int n)
{
  if (n != 2) {
    LOG(FATAL) << "Two parameters are needed : <patient> <dcm_filename>, but you provide "
               << n << " parameter(s)";
  }
  patient_name_ = inputs[0];
  filename_ = inputs[1];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::AddTimePointCommand::Run()
{
  // Check database
  if (db_ == NULL) {
    LOG(FATAL) << "A ClinicalTrialDatabase is needed in AddTimePointCommand. Aborting.";
  }

  if (tpdb_ == NULL) {
    LOG(FATAL) << "A TimePointsDatabase is needed in AddTimePointCommand. Aborting.";
  }

  // Get the new patient
  if (db_->GetIfExist<Patient>(odb::query<Patient>::name == patient_name_, patient_)) {
    db_->CheckPatient(patient_);
  }
  else {
    LOG(FATAL) << "Error, the patient " << patient_name_ << " does not exist";
  }

  Run(filename_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::AddTimePointCommand::Run(std::string filename)
{
  DD(filename);



}
// --------------------------------------------------------------------
