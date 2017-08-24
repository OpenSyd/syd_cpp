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
#include "sydInsertRoiMaskImageFromDicomStruct_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydDicomStructHelper.h"
#include "sydRoiMaskImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"
#include "sydCommonGengetopt.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertRoiMaskImageFromDicomStruct, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db =
    m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the dicom struct
  syd::IdType dicom_id = atoi(args_info.inputs[0]);
  auto dicom_struct = db->QueryOne<syd::DicomStruct>(dicom_id);
  DD(dicom_struct);

  // Get the image
  syd::IdType id = atoi(args_info.inputs[1]);
  auto image = db->QueryOne<syd::Image>(id);
  DD(image);

  // Get list of rois to create
  if (args_info.roi_id_given != args_info.roi_type_given) {
    LOG(FATAL) << "You must give the same number of --dicom_roi_id and --roi";
  }
  if (args_info.roi_id_given == 0) {
    LOG(FATAL) << "At least one --dicom_roi_id and --roi needed"; // FIXME LATER TO REMOVE
  }

  // Create list of roi ids
  std::vector<int> roi_ids;
  for(auto i=0; i<args_info.roi_id_given; i++)
    roi_ids.push_back(args_info.roi_id_arg[i]);
  DDS(roi_ids);

  // Create list of roitype
  syd::RoiType::vector roi_types;
  for(auto i=0; i<args_info.roi_type_given; i++) {
    auto roi_type = syd::FindRoiType(args_info.roi_type_arg[i], db);
    roi_types.push_back(roi_type);
  }
  DDS(roi_types);

  // Read image header
  auto image_header = syd::ReadImageHeader(image->GetAbsolutePath());

  // Loop
  for(auto i=0; i<roi_ids.size(); i++) {
    DD(i);
    auto mask = syd::InsertRoiMaskImageFromDicomStruct(dicom_struct, image_header, roi_ids[i], roi_types[i]);
    DD(mask);

    // update user info
    syd::AddTag(mask->tags, image->tags);
    syd::SetImageInfoFromCommandLine(mask, args_info);
    syd::SetTagsFromCommandLine(mask->tags, db, args_info);
    syd::SetCommentsFromCommandLine(mask->comments, db, args_info);
    db->Update(mask);
    LOG(1) << "Insert RoiMaskImage " << mask;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
