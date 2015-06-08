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
#include "sydImage.h"
#include "sydDatabase.h"
#include "sydTable.h"
#include "sydFile.h"
#include "sydFile-odb.hxx"
#include "sydRoiMaskImage.h"
#include "sydRoiMaskImage-odb.hxx"

// --------------------------------------------------------------------
syd::Image::Image():TableElementBase()
{
  for(auto &s:size) s = 0;
  for(auto &s:spacing) s = 1.0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Image::ToString() const
{
  std::string name;
  if (patient == NULL) name = "patient_not_set";
  else name = patient->name;
  std::string t;
  if (tag == NULL) t = "tag_not_set";
  else t = tag->label;
  std::stringstream ss ;
  ss << id << " "
     << name << " ";
  if (files.size() == 0) ss << "(no files) ";
  for(auto f:files) ss << f->filename << " ";
  ss << t << " " << type << " " << pixel_type << " "
     << size[0] << "x" << size[1] << "x" << size[2] << " "
     << spacing[0] << "x" << spacing[1] << "x" << spacing[2];
  if (dicoms.size() > 0) ss << " " << dicoms[0]->dicom_modality << " ";
  for(auto d:dicoms) ss << d->id << " ";
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
bool syd::Image::operator==(const Image & p)
{
  bool b = id == p.id and
    *patient == *p.patient;
  if (!b) return b;
  if (files.size() != p.files.size()) return false;
  for(auto i=0; i< files.size(); i++)
    b = b and (*files[i] == *p.files[i]);
  b = b and *tag == *p.tag;
  return b;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::OnDelete(syd::Database * db)
{
  for(auto f:files) db->AddToDeleteList(*f);

  // Also delete the RoiMaskImage
  std::vector<syd::RoiMaskImage> masks;
  db->Query<syd::RoiMaskImage>(odb::query<syd::RoiMaskImage>::image == id, masks);
  for(auto i:masks) db->AddToDeleteList(i);
}
// --------------------------------------------------
