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
#include "sydInsertCalibratedImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydRoiStatisticBuilder.h"
#include "sydCommonGengetopt.h"
#include "sydScaleImageBuilder.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{  // Init
  SYD_INIT_GGO(sydInsertCalibratedImage, 3);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the pixel value unit
  std::string pvu = args_info.inputs[0];
  syd::PixelValueUnit::pointer unit;
  odb::query<syd::PixelValueUnit> q = odb::query<syd::PixelValueUnit>::name == pvu;
  db->QueryOne(unit, q);

  // Get the calibration name
  std::string calib_tag = args_info.inputs[1];

  // Get the image to calibrate
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids); // Read the standard input if pipe
  for(auto i=2; i<args_info.inputs_num; i++)
    ids.push_back(atoi(args_info.inputs[i]));
  syd::Image::vector images;
  db->Query(images, ids);

  // Loop on the images to calibrate
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  syd::ImageBuilder builder(db);
  for(auto image:images) {
    // Get the corresponding calibration
    image->FatalIfNoDicom();
    auto injection = image->injection;
    syd::Calibration::pointer calibration = db->FindCalibration(image, calib_tag);
    double s = 1.0/calibration->factor / injection->activity_in_MBq;
    // Create an (empty) copy
    auto output = builder.NewMHDImageLike(image);
    db->UpdateTagsFromCommandLine(output->tags, args_info);
    output->pixel_value_unit = unit;
    // Load the itk image
    ImageType::Pointer itk_image = syd::ReadImage<ImageType>(db->GetAbsolutePath(image));
    // Scale the itk image
    syd::ScaleImage<ImageType>(itk_image, s);
    // Insert itk_image
    builder.SetImage<PixelType>(output, itk_image);
    builder.InsertAndRename(output);
    LOG(1) << "Create scaled (" << s << ") image: " << output;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
