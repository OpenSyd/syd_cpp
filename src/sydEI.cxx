

// Explicit Instantiation - Image types
#include <clitkImageCommon.h>

template class itk::Image<float,3>;
template class itk::Image<unsigned char,3>;

CLITK_READ_IMAGE(float, 3,);
CLITK_READ_IMAGE(unsigned char, 3,);


// Explicit Instantiation - Resample
#include <clitkResampleImageWithOptionsFilter.h>
CLITK_RESAMPLE_IMAGE_LIKE(float, 3, );
CLITK_RESAMPLE_IMAGE_LIKE(unsigned char, 3, );

// Explicit Instantiation - Crop
#include <clitkCropLikeImageFilter.h>
CLITK_CROP_LIKE(float, 3, );


// Explicit Instantiation - stat
#include <itkLabelStatisticsImageFilter.h>
template class
itk::LabelStatisticsImageFilter<itk::Image<float, 3>, itk::Image<unsigned char, 3> >;
