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
  syd::Image::pointer output;
  db->New(output); // empty image
  output->patient = patient;

  // set properties
  if (args_info.like_given) {
    syd::ImageHelper::CopyInformation(output, like);
  }
  else {
    syd::ImageHelper::UpdateImagePropertiesFromCommandLine(output, args_info);
  }

  // Update the tags
  db->UpdateTagsFromCommandLine(output->tags, args_info);
  db->Insert(output);
  syd::ImageHelper::InsertMhdFiles(output, filename);

  LOG(1) << "Inserting Image: " << output;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
