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
#include "sydDatabase.h"
#include "sydTable.h"
#include "sydInjection.h"
#include "sydInjection-odb.hxx"

// --------------------------------------------------
syd::Patient::Patient():TableElementBase()
{
  // default value
  id = 0; // will be changed when persist
  name = "not_set"; // must be unique
  study_id = 0; // must be unique
  weight_in_kg = 0;
  dicom_patientid = "unknown_dicom_id";
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::Set(std::string pname, IdType studyId, double weight)
{
  // default value
  name = pname;
  study_id = studyId;
  weight_in_kg = weight;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::Set(std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    LOG(FATAL) << "To insert patient, please set <name> <study_id> [<weight_in_kg> <dicom_patientid>]";
  }
  name = arg[0];
  study_id = atoi(arg[1].c_str());
  if (arg.size() > 2) weight_in_kg = atof(arg[2].c_str());
  if (arg.size() > 3) dicom_patientid = arg[3];
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
bool syd::Patient::operator==(const Patient & p)
{
  return (id == p.id and
          name == p.name and
          study_id == p.study_id and
          weight_in_kg == p.weight_in_kg);
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
void syd::Patient::OnDelete(syd::Database * db)
{
  std::vector<syd::Injection> injections;
  db->Query<syd::Injection>(odb::query<syd::Injection>::patient == id, injections);
  for(auto i:injections) db->AddToDeleteList(i);
}
// --------------------------------------------------
