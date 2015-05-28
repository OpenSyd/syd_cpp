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
template<class ImageType>
void ImageBuilder::UpdateImageInfo(syd::Image & image, typename ImageType::Pointer & itk_image)
{
  // Check dimension
  if (itk_image->GetImageDimension() != 3) {
    EXCEPTION("Cannot create syd::Image from itk image because dimension is "
              << itk_image->GetImageDimension() << " while only 3D is (yet) allowed.");
  }
  image.dimension = itk_image->GetImageDimension();

  //   type, size, spacing, dimension
  typedef typename ImageType::PixelType PixelType;
  if (typeid(PixelType) == typeid(float)) image.pixel_type = "float";
  else {
    if (typeid(PixelType) == typeid(short)) image.pixel_type = "short";
    else {
      EXCEPTION("Cannot create syd::Image from itk image with pixeltype = " << typeid(PixelType).name());
    }
  }

  // Size
  typedef typename ImageType::RegionType RegionType;
  const RegionType & region = itk_image->GetLargestPossibleRegion();
  image.size[0] = region.GetSize()[0];
  image.size[1] = region.GetSize()[1];
  image.size[2] = region.GetSize()[2];

  // Spacing
  image.spacing[0] = itk_image->GetSpacing()[0];
  image.spacing[1] = itk_image->GetSpacing()[1];
  image.spacing[2] = itk_image->GetSpacing()[2];
}
// --------------------------------------------------------------------
