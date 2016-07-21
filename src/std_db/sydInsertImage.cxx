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
#include "sydInsertImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
//#include "sydImageFromDicomBuilder.h"
#include "sydCommonGengetopt.h"
#include "sydImageHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertImage, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the mhd filename
  std::string filename = args_info.inputs[0];

  // Check options
  if (args_info.like_given && args_info.patient_given) {
    LOG(FATAL) << "Please, only set either --like or --patient, not both.";
  }
  if (!args_info.like_given && !args_info.patient_given) {
    LOG(FATAL) << "Please, set --like or --patient (but not both).";
  }

  // Get the patient
  syd::Image::pointer like;
  syd::Patient::pointer patient;
  if (args_info.like_given) {
    syd::IdType id = args_info.like_arg;
    db->QueryOne(like, id);
    patient = like->patient;
  }
  else {
    patient = db->FindPatient(std::string(args_info.patient_arg));
  }

  // create a new image
  //  syd::ImageBuilder builder(db);
  syd::Image::pointer output;
  db->New(output); // empty image
  output->patient = patient;
  syd::ImageHelper::CopyAndSetMhdImage(output, filename);

  // set properties
  if (args_info.like_given) {
    syd::ImageHelper::CopyInformation(output, like);
  }
  else {
    if (args_info.pixel_unit_given)
      syd::ImageHelper::SetPixelUnit(output, args_info.pixel_unit_arg);
    //    output = builder.NewMHDImage(patient, filename);
    //    if (args_info.pixel_unit_given)
    //  builder.SetPixelType(args_info.pixel_unit_arg);
    // Try options, if error remove temporary files
  }
  DD(output);

  //  builder.UpdateFromCommandLine(output, args_info); FIXME
  db->UpdateTagsFromCommandLine(output->tags, args_info);
  // syd::TagHelper::UpdateTagsFromCommandLine(output, args_info); ?

  // insert in the db (make it persistent)
  //builder.Insert(output); // auto rename files ?

  db->Insert(output); // auto rename files ? FIXME check without insert to test temp file

  //  builder.InsertAndRename(output); // TO REMOVE --> in pre_persist


  LOG(1) << "Inserting Image " << output;
  // This is the end, my friend.

  /*

  // Get the image to copy info from
  if (!args_info.like_given) {
  LOG(FATAL) << "Error flag 'like' is mandatory (yet).";
  }
  syd::IdType id = args_info.like_arg;
  syd::Image::pointer input;
  db->QueryOne(input, id);

  // Create the new image
  syd::ImageBuilder builder(db);
  syd::Image::pointer output = builder.NewMHDImageLike(input);
  builder.CopyImageFromFile(output, filename);

  // Add optional tag
  db->UpdateTagsFromCommandLine(output->tags, args_info);

  // Set optional unity
  if (args_info.pixelunit_given) {
  syd::PixelValueUnit::pointer unit;
  unit = db->FindPixelValueUnit(args_info.pixelunit_arg);
  output->pixel_value_unit = unit;
  }

  // Insert in the db
  builder.InsertAndRename(output);
  */
}
// --------------------------------------------------------------------
