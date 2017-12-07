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

#include "itkSumProjectionImageFilter.h"
#include "itkShiftScaleImageFilter.h"
#include "itkFlipImageFilter.h"
#include "itkPermuteAxesImageFilter.h"
#include "sydImageUtils.h"

//--------------------------------------------------------------------
template<class ImageType, class OutputImageType>
typename OutputImageType::Pointer
syd::Projection(const ImageType * input,
                double dimension,
                bool mean,
                bool flip)
{
  //Project the image along dimension
  auto projection = syd::Projection<ImageType, OutputImageType>(input, dimension);

  //Compute the mean if the flag is on
  if(mean) {
    double size;
    size = input->GetLargestPossibleRegion().GetSize(dimension);

    typedef itk::ShiftScaleImageFilter <OutputImageType, OutputImageType > DivideImageFilterType;
    typename DivideImageFilterType::Pointer divideImageFilter = DivideImageFilterType::New ();
    divideImageFilter->SetInput(projection);
    divideImageFilter->SetScale(1/size);
    divideImageFilter->Update();
    projection = divideImageFilter->GetOutput();
  }

  //Flip the image in order to have the head at the top and the feet at the bottom (flag ? car pas intéressant tout le temps)
  if(flip) {
    if (dimension == 0) {
      //I don't want to use AffineRegistration because I don't know the center of rotation but now it's more complicated:
      //I wanted to use PermuteAxesImageFilter but to have the correct orientation (ie. head at the top),
      //I have to use FlipImageFilter twice.
      //And PermuteAxesImageFilter save the image with an wrong direction matrix, so finally I have to correct it

      itk::FixedArray<bool, 2> flipAxes;
      flipAxes[0] = false;
      flipAxes[1] = true;

      typedef itk::FlipImageFilter <OutputImageType> FlipImageFilterType;
      typename FlipImageFilterType::Pointer flipFilter = FlipImageFilterType::New();
      flipFilter->SetInput(projection);
      flipFilter->SetFlipAxes(flipAxes);
      if (projection->GetSpacing()[0] < 0)
        flipFilter->SetCoordinateTolerance(-flipFilter->GetCoordinateTolerance());
      flipFilter->Update();
      projection = flipFilter->GetOutput();

      typedef itk::PermuteAxesImageFilter<OutputImageType> PermuteAxesImageFilterType;
      itk::FixedArray<unsigned int, 2> order;
      order[0] = 1;
      order[1] = 0;

      typename PermuteAxesImageFilterType::Pointer permuteAxesFilter = PermuteAxesImageFilterType::New();
      permuteAxesFilter->SetInput(projection);
      permuteAxesFilter->SetOrder(order);
      if (projection->GetSpacing()[0] < 0)
        permuteAxesFilter->SetCoordinateTolerance(-permuteAxesFilter->GetCoordinateTolerance());
      permuteAxesFilter->Update();
      projection = permuteAxesFilter->GetOutput();

      typename FlipImageFilterType::Pointer flipFilter2 = FlipImageFilterType::New();
      flipFilter2->SetInput(projection);
      flipFilter2->SetFlipAxes(flipAxes);
      if (projection->GetSpacing()[0] < 0)
        flipFilter2->SetCoordinateTolerance(-flipFilter2->GetCoordinateTolerance());
      flipFilter2->Update();
      projection = flipFilter2->GetOutput();

      typename OutputImageType::DirectionType matrix;
      matrix[0][0] = 1;
      matrix[1][0] = 0;
      matrix[0][1] = 0;
      matrix[1][1] = 1;
      projection->SetDirection(matrix);

    }
    else if (dimension == 1) {
      //A single flip around y-axis
      itk::FixedArray<bool, 2> flipAxes;
      flipAxes[0] = false;
      flipAxes[1] = true;

      typedef itk::FlipImageFilter <OutputImageType> FlipImageFilterType;
      typename FlipImageFilterType::Pointer flipFilter = FlipImageFilterType::New ();
      flipFilter->DebugOn();
      flipFilter->SetInput(projection);
      flipFilter->SetFlipAxes(flipAxes);
      if (projection->GetSpacing()[0] < 0)
        flipFilter->SetCoordinateTolerance(-flipFilter->GetCoordinateTolerance());
      flipFilter->Update();
      projection = flipFilter->GetOutput();
    }
  }

  return projection;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
template<class ImageType, class OutputImageType>
typename OutputImageType::Pointer
syd::Projection(const ImageType * input, double dimension)
{

  //typedef typename ImageType::PixelType PixelType;
  //const int Dim = ImageType::ImageDimension;
  //typedef itk::Image<PixelType,Dim-1> OutputImageType;

  // Filter
  typedef itk::SumProjectionImageFilter<ImageType,OutputImageType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetProjectionDimension(dimension);
  filter->SetInput(input);
  filter->Update();
  return filter->GetOutput();

}
//--------------------------------------------------------------------
