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
syd::ImageTransform::ImageTransform():Record()
{
  date = "unset";
  fixed_image = NULL;
  moving_image = NULL;
  fixed_mask = NULL;
  moving_mask = NULL;
  config_file = NULL;
  transform_file = NULL;
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


// --------------------------------------------------
bool syd::ImageTransform::IsEqual(const pointer p) const
{
  bool b = syd::Record::IsEqual(p) and
    fixed_image->id == p->fixed_image->id and
    moving_image->id == p->moving_image->id and
    fixed_mask->id == p->fixed_mask->id and
    moving_mask->id == p->moving_mask->id and
    config_file->id == p->config_file->id and
    transform_file->id == p->transform_file->id and
    date == p->date;
  if (!b) return b;
  for(auto i=0; i< tags.size(); i++)  b = b and (tags[i]->id == p->tags[i]->id); // if not same order ?
  return b;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::ImageTransform::CopyFrom(const pointer p)
{
  syd::Record::CopyFrom(p);
  fixed_image = p->fixed_image;
  moving_image = p->moving_image;
  fixed_mask = p->fixed_mask;
  moving_mask = p->moving_mask;
  config_file = p->config_file;
  transform_file = p->transform_file;
  date = p->date;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::ImageTransform::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  LOG(FATAL) << "Set ImageTransform not implemented";
}
// --------------------------------------------------


// --------------------------------------------------
void syd::ImageTransform::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  LOG(FATAL) << "InitPrintTable ImageTransform not implemented";
}
// --------------------------------------------------


// --------------------------------------------------
void syd::ImageTransform::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  LOG(FATAL) << "InitPrintTable ImageTransform not implemented";
}
// --------------------------------------------------



// --------------------------------------------------
// std::string syd::ImageTransform::ComputeRelativeFolder() const
// {
//    LOG(FATAL) << "ComputeRelativeFolder ImageTransform not implemented";return "";
// }
// --------------------------------------------------
