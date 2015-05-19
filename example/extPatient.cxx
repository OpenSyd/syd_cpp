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

// --------------------------------------------------------------------
ext::Patient::Patient():syd::Patient()
{
  birth_date = "birth_date";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void ext::Patient::Set(std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    LOG(FATAL) << "To insert patient, please set <name> <study_id> [<weight_in_kg> <dicom_patientid> <birth_date>]";
  }
  syd::Patient::Set(arg);
  if (arg.size() > 4) {
    std::string pdate = arg[4];
    if (!syd::IsDateValid(arg[4])) {
      LOG(FATAL) << "Error while using extPatient::Set, the date is not valid: " << pdate;
    }
    birth_date = pdate;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string ext::Patient::ToString() const
{
  std::stringstream ss ;
  ss << syd::Patient::ToString() << " "
     << birth_date;
  return ss.str();
}
// --------------------------------------------------------------------
