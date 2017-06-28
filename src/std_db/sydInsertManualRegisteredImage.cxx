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
#include "sydInsertManualRegisteredImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommonGengetopt.h"
#include "sydManualRegistration.h"
#include <numeric>

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertManualRegisteredImage, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of images id
  syd::IdType idImage = atoi(args_info.inputs[0]);
  syd::Image::pointer inputImage;
  db->QueryOne(inputImage, idImage); // will fail if not found
  LOG(2) << "Read image :" << inputImage;

  //Get x, y, z displacement
  double x(0), y(0), z(0);
  if (args_info.x_given)
    x = args_info.x_arg;
  if (args_info.y_given)
    y = args_info.y_arg;
  if (args_info.z_given)
    z = args_info.z_arg;
  syd::Image::pointer image;

  //Center the 2 images (if needed)
  if (args_info.center_given) {
    syd::IdType idCenterImage = args_info.center_arg;
    syd::Image::pointer centerImage;
    db->QueryOne(centerImage, idCenterImage); // will fail if not found
    LOG(2) << "Read image :" << centerImage;
    typedef float PixelType;
    typedef itk::Image<PixelType, 3> ImageType3D;
    auto itk_centerImage = syd::ReadImage<ImageType3D>(centerImage->GetAbsolutePath());
    auto itk_inputImage = syd::ReadImage<ImageType3D>(inputImage->GetAbsolutePath());

    double xCenter(0), yCenter(0), zCenter(0);
    xCenter = itk_centerImage->GetOrigin()[0] + itk_centerImage->GetLargestPossibleRegion().GetSize()[0]*itk_centerImage->GetSpacing()[0]/2 - itk_inputImage->GetOrigin()[0] - itk_inputImage->GetLargestPossibleRegion().GetSize()[0]*itk_inputImage->GetSpacing()[0]/2 +x;
    yCenter = itk_centerImage->GetOrigin()[1] + itk_centerImage->GetLargestPossibleRegion().GetSize()[1]*itk_centerImage->GetSpacing()[1]/2 - itk_inputImage->GetOrigin()[1] - itk_inputImage->GetLargestPossibleRegion().GetSize()[1]*itk_inputImage->GetSpacing()[1]/2 +y;
    zCenter = itk_centerImage->GetOrigin()[2] + itk_centerImage->GetLargestPossibleRegion().GetSize()[2]*itk_centerImage->GetSpacing()[2]/2 - itk_inputImage->GetOrigin()[2] - itk_inputImage->GetLargestPossibleRegion().GetSize()[2]*itk_inputImage->GetSpacing()[2]/2 +z;
    image = syd::InsertManualRegistration(inputImage, xCenter, yCenter, zCenter);
  }
  else {
    // Just main computation
    image = syd::InsertManualRegistration(inputImage, x, y, z);
  }

  // Update image info
  syd::SetImageInfoFromImage(image, inputImage);
  syd::SetImageInfoFromFile(image);
  syd::SetTagsFromCommandLine(image->tags, db, args_info);
  syd::SetImageInfoFromCommandLine(image, args_info);
  db->Update(image);
  LOG(1) << "Inserting Image " << image;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
