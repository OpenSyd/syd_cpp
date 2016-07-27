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
#include "sydCalibration.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
syd::Calibration::Calibration():
  syd::Record(),
  syd::RecordWithTags()
{
  image = NULL;
  factor = 1.0;
  fov_ratio = 1.0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Calibration::ToString() const
{
  std::stringstream ss ;
  std::string name;
  if (image == NULL) name = empty_value;
  else {
    if (image->patient == NULL) name = empty_value;
    name = image->patient->name;
  }
  ss << id << " "
     << name << " "
     << (image==NULL? empty_value:syd::ToString(image->id)) << " ";
  for(auto t:tags) ss << t->label << " ";
  ss << factor << " " << fov_ratio;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
void syd::Calibration::DumpInTable(syd::PrintTable & ta) const
{
  std::string iname;
  std::string pname;
  if (image == NULL) { iname = empty_value; pname = empty_value; }
  else {
    iname = syd::ToString(image->id);
    if (image->patient == NULL) pname = empty_value;
    pname = image->patient->name;
  }
  ta.Set("id", id);
  ta.Set("p", pname);
  ta.Set("image", iname);
  ta.Set("tags", GetLabels(tags));
  ta.Set("factor", factor, 8);
  ta.Set("fov_ratio", fov_ratio, 3);
}
// --------------------------------------------------
