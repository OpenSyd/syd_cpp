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
syd::Calibration::Calibration():syd::Record()
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
void syd::Calibration::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  LOG(FATAL) << "To insert Calibration ... TODO ";
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Calibration::CopyFrom(const pointer p)
{
  syd::Record::CopyFrom(p);
  image = p->image;
  for(auto t:p->tags) tags.push_back(t);
  factor = p->factor;
  fov_ratio = p->fov_ratio;
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::Calibration::IsEqual(const pointer p) const
{
  bool b = (syd::Record::IsEqual(p) and
            image->id == p->image->id);
  for(auto i=0; i<tags.size(); i++) b = b and tags[i]->id == p->tags[i]->id;
  b  =  b and factor == p->factor and fov_ratio == p->fov_ratio;
  return b;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Calibration::AddTag(syd::Tag::pointer tag)
{
  bool found = false;
  int i=0;
  while (i<tags.size() and !found) {
    if (tags[i]->label == tag->label) found = true;
    ++i;
  }
  if (!found) tags.push_back(tag);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Calibration::RemoveTag(syd::Tag::pointer tag)
{
  bool found = false;
  int i=0;
  while (i<tags.size() and !found) {
    if (tags[i]->label == tag->label) {
      found = true;
      tags.erase(tags.begin()+i);
    }
    ++i;
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Calibration::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  if (format == "help") {
    std::cout << "Available formats for table 'Calibration': default " << std::endl;
    return;
  }
  ta.AddColumn("#id", 5);
  ta.AddColumn("p", 8);
  ta.AddColumn("image", 8);
  ta.AddColumn("tags", 20);
  ta.AddColumn("factor", 10,2);
  ta.AddColumn("fov_ratio", 10,2);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Calibration::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  std::string iname;
  std::string pname;
  if (image == NULL) { iname = "image_unset"; pname = "patient_unset"; }
  else {
    iname = syd::ToString(image->id);
    if (image->patient == NULL) pname = "patient_unset";
    pname = image->patient->name;
  }
  ta << id << pname
     << iname << GetLabels(tags)
     << factor << fov_ratio;
}
// --------------------------------------------------
