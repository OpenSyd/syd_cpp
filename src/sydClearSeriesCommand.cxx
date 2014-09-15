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
#include "sydClearSeriesCommand.h"

// --------------------------------------------------------------------
void syd::ClearSeriesCommand::SetArgs(char ** inputs, int n)
{
  if (n != 1) {
    LOG(FATAL) << "One parameter is needed <patient> but you provide " << n << " parameter(s).";
  }
  patient_name_ = inputs[0];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ClearSeriesCommand::Run()
{
  // Check database
  if (databases_.size() != 1) {
    LOG(FATAL) << "A (single) database of type ClinicalTrialDatabase "
               << "is needed in ClearSeriesCommand. Aborting.";
  }
  db = static_cast<ClinicalTrialDatabase*>(databases_[0]);

  // Check patient
  if (!db->GetIfExist<Patient>(odb::query<Patient>::name == patient_name_, patient_)) {
    LOG(FATAL) << "Error, the patient " << patient_name_ << " does not exist in the db.";
  }
  db->CheckPatient(patient_);

  std::vector<Serie> series;
  db->LoadVector<Serie>(series, odb::query<Serie>::patient_id == patient_.id);
  int n = series.size();
  for(auto i=series.begin(); i<series.end(); i++) db->Erase(*i);
  VLOG(1) << "Delete all series ( " << n << ") for patient " << patient_.name;
}
// --------------------------------------------------------------------
