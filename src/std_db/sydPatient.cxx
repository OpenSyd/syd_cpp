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
  //  DD("syd::Patient::ToString");
  std::stringstream ss ;
  ss << id << " "
     << study_id << " "
     << name << " "
     << weight_in_kg << " "
     << dicom_patientid;
  return ss.str();
}
// --------------------------------------------------
