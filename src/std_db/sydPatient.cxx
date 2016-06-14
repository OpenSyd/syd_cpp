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
#include "sydPatient.h"
#include "sydInjection.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------
syd::Patient::Patient():syd::Record()
{
  // default value
  name = "unset"; // must be unique
  study_id = 0; // must be unique
  weight_in_kg = 0;
  dicom_patientid = "unset";
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::Patient::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << study_id << " "
     << name << " "
     << weight_in_kg << " "
     << dicom_patientid;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::Set(const std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    LOG(FATAL) << "To insert patient, please set <name> <study_id> "
               << "[<weight_in_kg> <dicom_patientid>]";
  }
  name = arg[0];
  study_id = atoi(arg[1].c_str());
  if (arg.size() > 2) weight_in_kg = atof(arg[2].c_str());
  if (arg.size() > 3) dicom_patientid = arg[3];
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::Set(const std::string & pname,
                       const IdType & pstudy_id,
                       const double pweight_in_kg,
                       const std::string pdicom_patientid)
{
  name = pname;
  study_id = pstudy_id;
  weight_in_kg = pweight_in_kg;
  dicom_patientid = pdicom_patientid;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::DumpInTable(syd::PrintTable2 & ta) const
{
  ta.Set("id",id);
  ta.Set("p", name);
  ta.Set("sid", study_id);
  ta.Set("w(kg)", weight_in_kg);
  ta.Set("dicom", dicom_patientid);
  syd::StandardDatabase* db = dynamic_cast<syd::StandardDatabase*>(db_);
  syd::Injection::vector injections;
  odb::query<syd::Injection> q = odb::query<syd::Injection>::patient == id;
  db->Query(injections, q);
  ta.Set("injection", injections.size());
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::Patient::CheckIdentity(std::string vdicom_patientid, std::string vdicom_name) const {
  if (dicom_patientid != vdicom_patientid) return false;
  // Try to guess initials. Consider the first letter and the first after the symbol '^'
  int n = vdicom_name.find("^");
  if (n != std::string::npos) {
    std::string initials = vdicom_name[0] + vdicom_name.substr(n+1,1);
    std::transform(initials.begin(), initials.end(), initials.begin(), ::tolower);
    // Check only 2 first letters
    if (initials[0] == name[0] and initials[1] == name[1]) return true;
    return false;
  }
  return false; // do not consider ok with a patient name not having a '^' inside
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::Patient::ComputeRelativeFolder() const
{
  return name;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event,db);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event,db);
}
// --------------------------------------------------
