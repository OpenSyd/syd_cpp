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
void ImageBuilder::SetImage(syd::Image::pointer image,
                            typename itk::Image<PixelType,3>::Pointer & itk_image)
{
  // Check dimension
  typedef itk::Image<PixelType,3> ImageType;
  auto dim = itk_image->GetImageDimension();
  if (dim != 3) {
    EXCEPTION("The dimension of the itk image must be 3, while it is " << dim);
  }

  // Check pixel type
  std::string pixeltype;
  if (typeid(PixelType) == typeid(float)) pixeltype = "float";
  else {
    if (typeid(PixelType) == typeid(short)) pixeltype = "short";
    else {
      if (typeid(PixelType) == typeid(unsigned char)) pixeltype = "uchar";
      else {
        EXCEPTION("Cannot create syd::Image from itk image with pixeltype = " << typeid(PixelType).name()
                  << ". Known types are 'float', 'short' and 'uchar'.");
      }
    }
  }

  // Check image type
  if (image->type != "mhd") {
    EXCEPTION("I only know 'mhd' as image type, while it is: " << image->type);
  }

  // Check files
  if (image->files.size() != 2) {
    EXCEPTION("Error while updating the image, I expect 2 Files: " << image);
  }

  // Update pixeltype and dimension
  image->pixel_type = pixeltype;
  image->dimension = dim;
  image->type = "mhd";

  // Flip if needed
  if (itk_image->GetDirection()[0][0] < 0) itk_image = syd::FlipImage<ImageType>(itk_image, 0);
  if (itk_image->GetDirection()[1][1] < 0) itk_image = syd::FlipImage<ImageType>(itk_image, 1);
  if (itk_image->GetDirection()[2][2] < 0) itk_image = syd::FlipImage<ImageType>(itk_image, 2);

  // Size
  typedef typename ImageType::RegionType RegionType;
  const RegionType & region = itk_image->GetLargestPossibleRegion();
  image->size[0] = region.GetSize()[0];
  image->size[1] = region.GetSize()[1];
  image->size[2] = region.GetSize()[2];

  // Spacing
  image->spacing[0] = itk_image->GetSpacing()[0];
  image->spacing[1] = itk_image->GetSpacing()[1];
  image->spacing[2] = itk_image->GetSpacing()[2];

  // Write Image (no need to update the File, only the content)
  syd::WriteImage<ImageType>(itk_image, db_->GetAbsolutePath(image));

  // MD5
  std::string md5 = syd::ComputeImageMD5<ImageType>(itk_image);
  image->files[1]->md5 = md5;
}
// --------------------------------------------------------------------
