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
#include "sydICRPOrganDose.h"
#include "sydTagHelper.h"
#include "sydStandardDatabase.h"
#include "sydRecordTraits.h"

DEFINE_TABLE_IMPL(ICRPOrganDose);

// --------------------------------------------------------------------
syd::ICRPOrganDose::ICRPOrganDose():
  syd::Record(),
  syd::RecordWithTags(),
  syd::RecordWithHistory(),
  syd::RecordWithMD5Signature()
{
  // target_fit_timepoints;
  // sources_fit_timepoints;
  // radionuclide;
  // S_coefficients
  // absorbed_dose_in_Gy;
  // phantom_name;
  // target_organ_name;
  // target_roitype;
  // source_organ_names;
  // source_roitypes;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ICRPOrganDose::ToString() const
{
  std::stringstream ss ;
  ss << id << " ";
  if (target_fit_timepoints == nullptr) {
    ss << "no target fit_timepoints";
    return ss.str();
  }
  ss << "T:" << target_fit_timepoints->id << " S:";
  for(auto s:sources_fit_timepoints)
    ss << s->id << " ";
  ss << syd::GetLabels(tags) << " "
     << radionuclide->name << " "
     << "d:" << absorbed_dose_in_Gy << " Gy "
     << "T:" << target_organ_name << " S:";
  for(auto s:source_organ_names)
    ss << s << " ";
  ss << phantom_name << " "
     << "Scoeff:";
  for(auto s:S_coefficients)
    ss << s << " ";
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ICRPOrganDose::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event, db);
  syd::RecordWithHistory::Callback(event, db, db_);
  syd::RecordWithMD5Signature::Callback(event, db, db_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ICRPOrganDose::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event, db);
  syd::RecordWithHistory::Callback(event, db, db_);
  syd::RecordWithMD5Signature::Callback(event, db, db_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ICRPOrganDose::ToStringForMD5() const
{
  std::stringstream ss;
  ss << target_fit_timepoints
     << radionuclide
     << absorbed_dose_in_Gy
     << phantom_name
     << target_organ_name;
  for(auto s:sources_fit_timepoints) ss << s;
  for(auto s:S_coefficients) ss << s;
  for(auto s:source_organ_names) ss << s;
  return ss.str();
}
// --------------------------------------------------------------------


