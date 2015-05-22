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
syd::Timepoint::~Timepoint() {}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Timepoint::Timepoint(const syd::Timepoint & other)
{
  copy(other);
}
// --------------------------------------------------------------------


// --------------------------------------------------
syd::Timepoint & syd::Timepoint::operator= (const syd::Timepoint & other) {
  if (this != &other) { copy(other); }
  return *this;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Timepoint::copy(const syd::Timepoint & tp)
{
  id = tp.id;
  tag = tp.tag;
  injection = tp.injection;
  time_from_injection_in_hours = tp.time_from_injection_in_hours;
  dicoms = tp.dicoms;
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
