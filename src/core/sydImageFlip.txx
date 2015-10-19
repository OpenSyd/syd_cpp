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
typename ImageType::Pointer FlipImage(const ImageType * input, int axe)
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
  flipFilter->SetFlipAboutOrigin(false);
  flipFilter->Update();
  return flipFilter->GetOutput();
}
//--------------------------------------------------------------------
