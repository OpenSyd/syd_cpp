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
#include "sydSubstituteRadionuclideImageBuilder.h"

// --------------------------------------------------------------------
syd::Image::pointer
syd::SubstituteRadionuclideImageBuilder::NewRadionuclideSubstitutedImage(syd::Image::pointer input,
                                                                         syd::Radionuclide::pointer rad)
{
  // Get information
  if (input->dicoms.size() < 1) {
    LOG(FATAL) << "Error this image is not associated with a dicom. ";
  }
  syd::DicomSerie::pointer dicom = input->dicoms[0];
  syd::Injection::pointer injection = dicom->injection;
  if (injection == NULL) {
    LOG(FATAL) << "Error this dicom is not associated with an injection :" << input->dicoms[0];
  }

  double time = syd::DateDifferenceInHours(dicom->acquisition_date, injection->date);
  double lambda = rad->GetLambdaInHours();

  // Create output image
  syd::Image::pointer result = NewMHDImageLike(input);

  // Change pixel values
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  ImageType::Pointer itk_image = syd::ReadImage<ImageType>(db_->GetAbsolutePath(input));
  itk::ImageRegionIterator<ImageType> iter(itk_image, itk_image->GetLargestPossibleRegion());
  while (!iter.IsAtEnd()) {
    iter.Set(iter.Get() * exp(-lambda * time));
    ++iter;
  }

  // Create and image file
  SetImage<PixelType>(result, itk_image);
  return result;
}
// --------------------------------------------------------------------
