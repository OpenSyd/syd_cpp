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
#include "sydElastix.h"
#include "sydStandardDatabase.h"
#include "sydFile.h"
#include "sydTagHelper.h"

DEFINE_TABLE_IMPL(Elastix);

// --------------------------------------------------------------------
syd::Elastix::Elastix():
  Record(),
  RecordWithHistory(),
  RecordWithTags(),
  RecordWithComments()
{
  fixed_image = nullptr;
  moving_image = nullptr;
  fixed_mask = nullptr;
  moving_mask = nullptr;
  config_file = nullptr;
  transform_file = nullptr;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Elastix::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << (fixed_image == nullptr ? empty_value:fixed_image->patient->name) << " "
     << (fixed_image == nullptr ? empty_value:std::to_string(fixed_image->id)) << " "
     << (moving_image == nullptr ? empty_value:std::to_string(moving_image->id)) << " "
     << syd::GetLabels(tags) << " "
     << (fixed_mask == nullptr ? empty_value:std::to_string(fixed_mask->id)) << " "
     << (moving_mask == nullptr ? empty_value:std::to_string(moving_mask->id)) << " "
     << (config_file == nullptr ? empty_value:config_file->filename) << " "
     << (transform_file == nullptr ? empty_value:transform_file->filename) << " "
     << GetAllComments();
  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Elastix::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event, db);

  if (event == odb::callback_event::post_erase) {
    if (config_file != nullptr) db.erase(config_file);
    if (transform_file != nullptr) db.erase(transform_file);
  }

  if (event == odb::callback_event::pre_persist) {
    if (config_file != nullptr) db.persist(config_file);
    if (transform_file != nullptr) db.persist(transform_file);
  }

  if (event == odb::callback_event::pre_update) {
    if (config_file != nullptr) db.update(config_file);
    if (transform_file != nullptr) db.update(transform_file);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::Elastix::Check() const
{
  syd::CheckResult r;
  if (config_file != nullptr) r.merge(config_file->Check());
  if (transform_file != nullptr) r.merge(transform_file->Check());
  r.merge(fixed_image->Check());
  r.merge(moving_image->Check());
  if (fixed_mask != nullptr) r.merge(fixed_mask->Check());
  if (moving_mask != nullptr) r.merge(moving_mask->Check());
  return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Elastix::ComputeDefaultFolder()
{
  if (!IsPersistent()) {
    EXCEPTION("Cannot Elastix::ComputeDefaultFolder because the record is not persistent.");
  }
  std::ostringstream oss;
  oss << fixed_image->patient->name << "/elastix/" << "transform_" << id;
  return oss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Elastix::ComputeDefaultFilename()
{
  return "TransformParameters.0.txt";
}
// --------------------------------------------------------------------
