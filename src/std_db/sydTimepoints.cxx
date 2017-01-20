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
#include "sydTimepoints.h"
#include "sydTagHelper.h"
#include "sydStandardDatabase.h"
#include "sydRecordTraits.h"

DEFINE_TABLE_IMPL(Timepoints);

// --------------------------------------------------------------------
syd::Timepoints::Timepoints():
  syd::Record(),
  syd::RecordWithTags(),
  syd::RecordWithHistory(),
  syd::RecordWithMD5Signature()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Timepoints::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << (patient == NULL ? "no_patient":patient->name) << " "
     << times.size() << " "
     << syd::GetLabels(tags) << " ";
  for(auto i=0; i<times.size(); i++)
    ss << times[i] << " " << values[i] << " ";
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Timepoints::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event, db);
  syd::RecordWithHistory::Callback(event, db, db_);
  syd::RecordWithMD5Signature::Callback(event, db, db_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Timepoints::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event, db);
  syd::RecordWithHistory::Callback(event, db, db_);
  syd::RecordWithMD5Signature::Callback(event, db, db_);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Timepoints::ToStringForMD5() const
{
  std::stringstream ss;
  ss << patient->id
     << injection->id;
  for(auto i=0; i<times.size(); i++) {
    ss << std::setprecision(30)
       << times[i] << values[i];
    if (std_deviations.size() > i) ss << std_deviations[i];
  }
  return ss.str();
}
// --------------------------------------------------------------------


