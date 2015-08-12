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
#include "sydCropImageBuilder.h"
#include "sydTable.h"
#include "sydImageUtils.h"

// --------------------------------------------------------------------
syd::CropImageBuilder::CropImageBuilder(syd::StandardDatabase * db):syd::CropImageBuilder()
{
  SetDatabase(db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CropImageBuilder::CropImageBuilder()
{
  db_ = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::CropImageBuilder::CropImageLike(syd::Image::pointer image,
                                          const syd::Image::pointer like,
                                          bool forceFlag)
{
  // Check
  if (!forceFlag) {
    if (image->frame_of_reference_uid != like->frame_of_reference_uid) {
      EXCEPTION("Cannot crop: " << image << std::endl << "like: "
                << like << std::endl
                << "because they dont share the same frame_of_reference_uid (coordinate system)."
                << "Use the 'force' flag to do it anyway.");
    }
  }

  // Crop the image and write it to disk
  if (image->pixel_type == "float") CropImageLike<float>(image, like, 1, 0);
  else if (image->pixel_type == "short") CropImageLike<short>(image, like, 1, -1000);
  else if (image->pixel_type == "unsigned char") CropImageLike<unsigned char>(image, like, 0, 0);
  else {
    LOG(FATAL) << "Unknown pixel_type: " << image->pixel_type;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::CropImageBuilder::CropImageWithThreshold(syd::Image::pointer image, double threshold)
{
  // Dispatch according to pixel type
  if (image->pixel_type == "float")  CropImageWithThreshold<float>(image, threshold);
  else if (image->pixel_type == "short")  CropImageWithThreshold<short>(image, threshold);
  else if (image->pixel_type == "unsigned char")  CropImageWithThreshold<unsigned char>(image, threshold);
  else {
    LOG(FATAL) << "Unknown pixel_type: " << image->pixel_type;
  }
}
// --------------------------------------------------------------------
