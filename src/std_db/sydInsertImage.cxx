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
  syd::ImageHelper::CopyAndSetMhdImage(output, filename);

  // set properties
  if (args_info.like_given) {
    syd::ImageHelper::CopyInformation(output, like);
  }
  else {
    if (args_info.pixel_unit_given)
      syd::ImageHelper::SetPixelUnit(output, args_info.pixel_unit_arg);
    if (args_info.modality_given) output->modality = args_info.modality_arg;
    if (args_info.frame_of_reference_uid_given)
      output->frame_of_reference_uid = args_info.frame_of_reference_uid_arg;
    if (args_info.acquisition_date_given) {
      auto d = args_info.acquisition_date_arg;
      if (!syd::IsDateValid(d)) {
        LOG(FATAL) << "Acquisition date is not valid.";
      }
      output->acquisition_date = d;
    }
    if (args_info.injection_given)
      syd::ImageHelper::SetInjection(output, args_info.injection_arg);
    if (args_info.dicom_given) {
      for(auto i=0; i<args_info.dicom_given; i++)
        syd::ImageHelper::AddDicomSerie(output, args_info.dicom_arg[i]);
    }
  }

  //  builder.UpdateFromCommandLine(output, args_info); FIXME
  db->UpdateTagsFromCommandLine(output->tags, args_info);
  // syd::TagHelper::UpdateTagsFromCommandLine(output, args_info); ?

  db->Insert(output);
  LOG(1) << "Inserting Image: " << output;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
