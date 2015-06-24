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
#include "sydImageTransform.h"
#include "sydDatabase.h"

// --------------------------------------------------------------------
syd::ImageTransform::ImageTransform():TableElementBase()
{
  date = "unknown_date";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ImageTransform::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << (fixed_image == NULL ? "no_patient":fixed_image->patient->name) << " "
     << (fixed_image == NULL ? "no_fixed_image":syd::ToString(fixed_image->id)) << " "
     << (moving_image == NULL ? "no_moving_image":syd::ToString(moving_image->id)) << " "
     << GetTagLabels(tags) << " "
     << (fixed_mask == NULL ? "no_fixed_mask":syd::ToString(fixed_mask->id)) << " "
     << (moving_mask == NULL ? "no_moving_mask":syd::ToString(moving_mask->id)) << " "
     << (config_file == NULL ? "no_config":config_file->filename) << " "
     << (transform_file == NULL ? "no_transform":transform_file->filename) << " "
     << date;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ImageTransform::ToLargeString() const
{
  // Add Large string of tags, files, images
  std::stringstream ss;
  ss << ToString() << " "
     << fixed_image->ToLargeString() << " "
     << moving_image->ToLargeString() << " ";
  for(auto & t:tags) ss << t->ToLargeString() << " ";
  if (fixed_mask != NULL) ss << fixed_mask->ToLargeString() << " ";
  if (moving_mask != NULL) ss << moving_mask->ToLargeString() << " ";
  if (config_file != NULL) ss << config_file->ToLargeString() << " ";
  if (transform_file != NULL) ss << transform_file->ToLargeString() << " ";
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::ImageTransform::operator==(const ImageTransform & p)
{
  bool b = id == p.id and
    *fixed_image == *p.fixed_image and
    *moving_image == *p.moving_image and
    *fixed_mask == *p.fixed_mask and
    *moving_mask == *p.moving_mask and
    *config_file == *p.config_file and
    *transform_file == *p.transform_file and
    date == p.date;
  if (!b) return b;
  for(auto i=0; i< tags.size(); i++)  b = b and (*tags[i] == *p.tags[i]); // if not same order ?
  return b;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::ImageTransform::OnDelete(syd::Database * db)
{
  DD("on delete ImageTransform");
  if (config_file != NULL) db->AddToDeleteList(*config_file);
  if (transform_file != NULL) db->AddToDeleteList(*transform_file);
}
// --------------------------------------------------
