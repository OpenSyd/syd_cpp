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

// ext
#include "extPatient.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------
ext::Patient::Patient():syd::Patient()
{
  birth_date = "unset_birth_date";
}
// --------------------------------------------------


// --------------------------------------------------
std::string ext::Patient::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << study_id << " "
     << name << " "
     << weight_in_kg << " "
     << dicom_patientid << " "
     << birth_date;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
bool ext::Patient::IsEqual(const pointer p) const
{
  return (syd::Patient::IsEqual(p) and birth_date == p->birth_date);
}
// --------------------------------------------------


// --------------------------------------------------
void ext::Patient::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  syd::Patient::Set(db, arg);
  if (arg.size() > 4) {
    if (!syd::IsDateValid(arg[4])) {
      LOG(FATAL) << "Error " << arg[4] << " is not a valid date. Use 'YYYY-MM-DD hh:mm' format, such as 2013-05-17 12:00.";
      }
    birth_date = arg[4];
  }
}
// --------------------------------------------------



// --------------------------------------------------
void ext::Patient::Set(const syd::Database * db,
                       const std::string & pname,
                       const syd::IdType & pstudy_id,
                       const double pweight_in_kg,
                       const std::string pdicom_patientid,
                       const std::string pbirth_date)
{
  syd::Patient::Set(db, pname, pstudy_id, pweight_in_kg, pdicom_patientid);
  birth_date = pbirth_date;
}
// --------------------------------------------------


// --------------------------------------------------
void ext::Patient::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  syd::Patient::InitPrintTable(db, ta, format);
  ta.AddColumn("birth", 20);
}
// --------------------------------------------------


// --------------------------------------------------
void ext::Patient::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  syd::Patient::DumpInTable(d, ta, format);
  ta << birth_date;
}
// --------------------------------------------------


// --------------------------------------------------
void ext::Patient::Callback(odb::callback_event event, odb::database & db) const
{
  //  DD("Callback const ext Patient");
  //DD(event);
  if (event == odb::callback_event::pre_load) {
    //DD("pre load");
  }
}
// --------------------------------------------------


// --------------------------------------------------
void ext::Patient::Callback(odb::callback_event event, odb::database & db)
{
  //  DD("Callback const ext Patient");
  //DD(event);
  if (event == odb::callback_event::pre_load) {
    //DD("pre load");
  }
}
// --------------------------------------------------
