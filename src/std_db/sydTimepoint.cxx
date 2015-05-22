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
#include "sydTimepoint.h"

// --------------------------------------------------------------------
syd::Timepoint::Timepoint():syd::TableElement()
{
  time_from_injection_in_hours = 0.0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Timepoint::SetValues(std::vector<std::string> & arg)
{
  LOG(FATAL) << "Could not insert a Timepoint with this function. Try sydInsertTimePoint.";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Timepoint::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << injection->patient->name << " "
     << tag->label << " "
     << injection->radionuclide->name << " "
     << time_from_injection_in_hours << "h "
     << dicoms.size();
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
bool syd::Timepoint::operator==(const Timepoint & p)
{
  bool b = (id == p.id and
            *injection == *p.injection and
            *tag == *p.tag and
            time_from_injection_in_hours == p.time_from_injection_in_hours);
  b = b and (dicoms.size() == p.dicoms.size());
  for(auto i=0; i<dicoms.size(); i++) {
    b = b and (*dicoms[i] == *p.dicoms[i]);
  }
  return b;
}
// --------------------------------------------------
