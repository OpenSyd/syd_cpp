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

// --------------------------------------------------------------------
template<class PixelType>
void CropImageBuilder::CropImageLike(syd::Image::pointer image,
                                     const syd::Image::pointer like,
                                     bool resampleFlag,
                                     int interpolationType,
                                     PixelType default_pixel)
{
  // load itk image
  typedef itk::Image<PixelType,3> ImageType;
  typename ImageType::Pointer itk_image =
    syd::ReadImage<ImageType>(db_->GetAbsolutePath(image));

  // load header of like image (no need to read all the pixels)
  auto reader = syd::ReadImageHeader(db_->GetAbsolutePath(like));
  auto itk_like = GetImageBase<3>(reader);

  // Crop image
  typename ImageType::Pointer output;
  if (resampleFlag)
    output = syd::ResampleAndCropImageLike<ImageType>(itk_image, itk_like, interpolationType, default_pixel);
  else
    output = syd::CropImageLike<ImageType>(itk_image, itk_like);

  // Replace image on disk
  syd::WriteImage<ImageType>(output, db_->GetAbsolutePath(image));

  // Update image information
  db_->UpdateImageInfo<PixelType>(image, output, true); // recompute md5
  db_->Update(image);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class PixelType>
void CropImageBuilder::CropImageWithThreshold(syd::Image::pointer image, double lower_threshold)
{
  // load itk image
  typedef itk::Image<PixelType,3> ImageType;
  typename ImageType::Pointer itk_image =
    syd::ReadImage<ImageType>(db_->GetAbsolutePath(image));

  // Perform crop
  auto output = syd::CropImageWithLowerThreshold<ImageType>(itk_image, lower_threshold);

  // Replace image on disk
  syd::WriteImage<ImageType>(output, db_->GetAbsolutePath(image));

  // Update image information
  db_->UpdateImageInfo<PixelType>(image, output, true); // recompute md5
  db_->Update(image);
}
// --------------------------------------------------------------------
