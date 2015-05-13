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

std::string syd::Patient::mname = "Patient";

// --------------------------------------------------
syd::Patient::Patient():TableElement()
{
  // default value
  id = 0; // will be changed when persist
  name = "anonymous"; // must be unique
  study_id = 0; // must be unique
  weight_in_kg = 0;
  dicom_patientid = "unknown_dicom_id";
}
// --------------------------------------------------


// --------------------------------------------------
syd::Patient::Patient(std::string pname, IdType studyId, double weight):Patient()
{
  Set(pname, studyId, weight);
}
// --------------------------------------------------


// --------------------------------------------------
syd::Patient::~Patient()
{
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
syd::Patient::Patient(const Patient & other):TableElement(other)
{
  copy(other);
}
// --------------------------------------------------


// --------------------------------------------------
syd::Patient & syd::Patient::operator= (const syd::Patient & other)
{
  if (this != &other) { copy(other); }
  return *this;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::copy(const syd::Patient & t) {
  id = t.id;
  name = t.name;
  study_id = t.study_id;
  weight_in_kg = t.weight_in_kg;
  dicom_patientid = t.dicom_patientid;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::SetValues(std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    LOG(FATAL) << "Provide <name> <study_id> <weight_in_kg> <dicom_patientid>. ";
  }
  if (arg[0] == "all") {
    LOG(FATAL) << "A patient name cannot be 'all', this is a reserved word for query.";
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
