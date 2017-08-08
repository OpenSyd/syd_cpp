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

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer syd::FlipImage(const ImageType * input, int axe, bool flipOrigin)
{
  // Filter
  typedef itk::FlipImageFilter<ImageType> FlipImageFilterType;
  auto flipFilter = FlipImageFilterType::New();
  auto dim = input->GetImageDimension();
  flipFilter->SetInput(input);
  if (axe>= dim || axe<0) {
    LOG(FATAL) << "Error axe should be >0 and <dimension, while it is " << axe;
  }
  typename FlipImageFilterType::FlipAxesArrayType axes;
  axes.Fill(false);
  axes[axe]=true;
  flipFilter->SetFlipAxes(axes);
  flipFilter->SetFlipAboutOrigin(flipOrigin);
  flipFilter->Update();
  return flipFilter->GetOutput();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType>
bool syd::FlipImageIfNegativeSpacing(typename ImageType::Pointer & image)
{
  bool flip = false;

  // Flip direction if the spacing is negative
  for(auto i=0; i<image->GetImageDimension(); i++) {
    if (image->GetSpacing()[i] < 0) {
      image = syd::FlipImage<ImageType>(image, i);
      flip = true;
    }
  }

  // then remove negative spacing and negative direction (due to flip)
  auto direction = image->GetDirection();
  auto spacing = image->GetSpacing();
  for(auto i=0; i<image->GetImageDimension(); i++) {
    if (image->GetSpacing()[i] < 0.0) {
      direction(i,i) = -direction(i,i);
      spacing[i] = -spacing[i];
    }
  }
  image->SetDirection(direction);
  image->SetSpacing(spacing);
  return flip;
}
//--------------------------------------------------------------------
