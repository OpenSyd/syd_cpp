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

#include "sydImageUtils.h"
#include "itkResampleImageFilter.h"
#include "itkTranslationTransform.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"

//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer
syd::ManualRegistration(const ImageType * inputImage, double x, double y, double z, bool translateOrigin)
{
  typedef itk::ResampleImageFilter<ImageType, ImageType> ResampleFilterType;
  typename ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
  resampleFilter->SetSize(inputImage->GetLargestPossibleRegion().GetSize());
  resampleFilter->SetInput(inputImage);
  resampleFilter->SetOutputSpacing(inputImage->GetSpacing());

  if(translateOrigin) //Just modify the origin of the image
  {
    // Instantiate the transform and specify it should be the id transform.
    typedef itk::IdentityTransform<double, 3> TransformType;
    TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();

    typename ImageType::PointType origin = inputImage->GetOrigin();
    origin[0] += x;
    origin[1] += y;
    origin[2] += z;

    resampleFilter->SetTransform(transform.GetPointer());
    resampleFilter->SetOutputOrigin(origin);
  }
  else //Translate the image while the origin stay unchanged
  {
    typedef itk::TranslationTransform<double,3> TranslationTransformType;
    TranslationTransformType::Pointer transform =
      TranslationTransformType::New();
    TranslationTransformType::OutputVectorType translation;
    translation[0] = x;
    translation[1] = y;
    translation[2] = z;
    transform->Translate(translation);

    resampleFilter->SetTransform(transform.GetPointer());
    resampleFilter->SetOutputOrigin(inputImage->GetOrigin());
  }

  resampleFilter->Update();
  return resampleFilter->GetOutput();
}
//--------------------------------------------------------------------

