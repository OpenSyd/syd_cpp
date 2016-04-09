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
  if (image == NULL) name = "image_unset";
  else {
    if (image->patient == NULL) name = "patient_unset";
    name = image->patient->name;
  }
  ss << id << " "
     << name << " "
     << (image==NULL? "image_unset":syd::ToString(image->id)) << " ";
  for(auto t:tags) ss << t->label << " ";
  ss << factor << " " << fov_ratio;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
void syd::Calibration::InitTable(syd::PrintTable & ta) const
{
  ta.AddColumn("id");
  ta.AddColumn("p");
  ta.AddColumn("image");
  ta.AddColumn("tags");
  auto & c = ta.AddColumn("factor");
  c.precision = 8;
  auto & cc = ta.AddColumn("fov_ratio");
  cc.precision = 3;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Calibration::DumpInTable(syd::PrintTable & ta) const
{
  std::string iname;
  std::string pname;
  if (image == NULL) { iname = "image_unset"; pname = "patient_unset"; }
  else {
    iname = syd::ToString(image->id);
    if (image->patient == NULL) pname = "patient_unset";
    pname = image->patient->name;
  }
  ta.Set("id", id);
  ta.Set("p", pname);
  ta.Set("image", iname);
  ta.Set("tags", GetLabels(tags));
  ta.Set("factor", factor);
  ta.Set("fov_ratio", fov_ratio);
}
// --------------------------------------------------
