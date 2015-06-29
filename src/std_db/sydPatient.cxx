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

// --------------------------------------------------
// syd::Patient::Patient():syd::Record()
// {
//   // default value
//   id = 0; // will be changed when persist
//   name = "unknown_name"; // must be unique
//   study_id = 0; // must be unique
//   weight_in_kg = 0;
//   dicom_patientid = "unknown_dicom_id";
// }
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
bool syd::Patient::IsEqual(const pointer p) const
{
  return (syd::Record::IsEqual(p) and name == p->name and
          study_id == p->study_id and dicom_patientid == p->dicom_patientid);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::Set(const syd::Database * db, const std::vector<std::string> & arg)
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
void syd::Patient::Set(const syd::Database * db,
                       const std::string & pname,
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
