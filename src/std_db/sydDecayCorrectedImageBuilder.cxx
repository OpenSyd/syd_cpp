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
#include "sydDecayCorrectedImageBuilder.h"

// --------------------------------------------------------------------
syd::Image::pointer
syd::DecayCorrectedImageBuilder::NewDecayCorrectedImage(syd::Image::pointer input)
{
  // Get dicom information
  if (input->dicoms.size() < 1)  EXCEPTION("This image is not associated with a dicom: " << input);

  syd::DicomSerie::pointer dicom = input->dicoms[0];
  syd::Injection::pointer injection = dicom->injection;
  if (injection == NULL) EXCEPTION("The associated dicom is not associated with an injection :" << input->dicoms[0]);

  double injected_activity = injection->activity_in_MBq;
  double time = syd::DateDifferenceInHours(dicom->acquisition_date, injection->date);
  double lambda = injection->GetLambdaInHours();

  // Scale the image
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  ImageType::Pointer itk_image = syd::ReadImage<ImageType>(db_->GetAbsolutePath(input));
  double f = exp(lambda * time); // decay correction: multiply by exp(lambda x time)
  syd::ScaleImage<ImageType>(itk_image, f);

  // Create output image
  syd::Image::pointer result = NewMHDImageLike(input);
  SetImage<PixelType>(result, itk_image);
  return result;
}
// --------------------------------------------------------------------
