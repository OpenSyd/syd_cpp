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
