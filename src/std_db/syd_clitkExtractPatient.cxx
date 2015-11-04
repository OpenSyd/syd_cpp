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
#include "syd_clitkExtractPatient_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydRoiMaskImageBuilder.h"
#include "sydCommonGengetopt.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(syd_clitkExtractPatient, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // Get the images
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids); // Read the standard input if pipe
  for(auto i=0; i<args_info.inputs_num; i++) ids.push_back(atoi(args_info.inputs[i]));

  syd::RoiType::pointer roitype = db->FindRoiType("body");

  // Loop on ids
  syd::Image::vector images;
  db->Query(images, ids);
  for(auto image:images) {
    if (image->dicoms.size() == 0) {
      LOG(FATAL) << "Error, not dicom associated with this image: " << image;
    }

    // Check if already exist // find mask roitype=body, image same dicom


    // Create a temporary file for the image
    std::string mhd_filename = syd::CreateTemporaryFile(db->GetDatabaseAbsoluteFolder(), ".mhd");

    // Create command line
    std::ostringstream cmd;
    cmd << "clitkExtractPatient "
        << " -i " << db->GetAbsolutePath(image)
        << " -o " << mhd_filename
        << " " << args_info.options_arg;
    LOG(1) << "Executing: " << std::endl << cmd.str();

    // Exec command line
    int r = syd::ExecuteCommandLine(cmd.str(), 2);

    // Stop if error in cmd
    if (r == -1) {
      LOG(FATAL) << "Error while executing the following command: " << std::endl << cmd.str();
    }

    // Second processing: morphological opening could help to remove table in some cases
    if (args_info.opening_given) {
      cmd.str("");
      cmd.clear();
      cmd << "clitkMorphoMath "
          << " -i " << mhd_filename
          << " -o " << mhd_filename
          << " -t3 -r3 ";
      LOG(1) << "Executing: " << std::endl << cmd.str();
      r = syd::ExecuteCommandLine(cmd.str(), 2);
    }

    // Stop if error in cmd
    if (r == -1) {
      LOG(FATAL) << "Error while executing the following command: " << std::endl << cmd.str();
    }

    // Create the mask image
    syd::DicomSerie::pointer dicom = image->dicoms[0];
    syd::RoiMaskImageBuilder b(db);
    try {
      syd::RoiMaskImage::pointer mask = b.InsertRoiMaskImage(dicom, roitype, mhd_filename);
      db->SetImageTagsFromCommandLine(mask, args_info);
      LOG(1) << "Inserting RoiMaskImage " << mask;
    }
    catch(std::exception & e) {
      LOG(WARNING) << "Cannot create mask image, skip it." << std::endl << e.what();
    }

    syd::DeleteMHDImage(mhd_filename);
  }
  // This is the end, my friend.
}
// --------------------------------------------------------------------
