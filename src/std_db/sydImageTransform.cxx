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
#include "sydStandardDatabase.h"
#include "sydFile.h"

// --------------------------------------------------------------------
syd::ImageTransform::ImageTransform()
  :Record(), RecordWithTags()
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
     << GetLabels(tags) << " "
     << (fixed_mask == NULL ? "no_fixed_mask":syd::ToString(fixed_mask->id)) << " "
     << (moving_mask == NULL ? "no_moving_mask":syd::ToString(moving_mask->id)) << " "
     << (config_file == NULL ? "no_config":config_file->filename) << " "
     << (transform_file == NULL ? "no_transform":transform_file->filename) << " "
     << date;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
void syd::ImageTransform::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event, db);

  if (event == odb::callback_event::post_erase) {
    if (config_file != NULL) db.erase(config_file);
    if (transform_file != NULL) db.erase(transform_file);
    std::string f = db_->ConvertToAbsolutePath(ComputeRelativeFolder());
    fs::remove_all(f);
  }

  if (event == odb::callback_event::pre_persist) {
    if (config_file !=NULL) db.persist(config_file);
    if (transform_file !=NULL) db.persist(transform_file);
  }

  if (event == odb::callback_event::pre_update) {
    if (config_file !=NULL) db.update(config_file);
    if (transform_file !=NULL) db.update(transform_file);
  }

}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::ImageTransform::ComputeRelativeFolder() const
{
  syd::Patient::pointer patient = fixed_image->patient;
  std::string p = patient->ComputeRelativeFolder()+PATH_SEPARATOR+"transform"+PATH_SEPARATOR+syd::ToString(id);
  return p;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::ImageTransform::InitTable(syd::PrintTable & ta) const
{
  ta.AddColumn("id");
  ta.AddColumn("p");
  ta.AddColumn("fixed");
  ta.AddColumn("moving");
  ta.AddColumn("config");
  ta.AddColumn("date");
  ta.AddColumn("ref_frame");
}
// --------------------------------------------------


// --------------------------------------------------
void syd::ImageTransform::DumpInTable(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("p", fixed_image->patient->name);
  ta.Set("fixed", fixed_image->id);
  ta.Set("moving", moving_image->id);
  ta.Set("config", (config_file == NULL ? "unset":config_file->filename));
  ta.Set("date", date);
  ta.Set("ref_frame", fixed_image->frame_of_reference_uid);
}
// --------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::ImageTransform::Check() const
{
  syd::CheckResult r;
  if (config_file != NULL) r.merge(config_file->Check());
  if (transform_file != NULL) r.merge(transform_file->Check());
  r.merge(fixed_image->Check());
  r.merge(moving_image->Check());
  if (fixed_mask != NULL) r.merge(fixed_mask->Check());
  if (moving_mask != NULL) r.merge(moving_mask->Check());
  return r;
}
// --------------------------------------------------------------------
