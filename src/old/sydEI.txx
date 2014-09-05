


// Explicit Instantiation - Image types
extern template class itk::Image<float,3>;
extern template class itk::Image<unsigned char,3>;

CLITK_READ_IMAGE(float, 3, extern);
CLITK_READ_IMAGE(unsigned char, 3, extern);


// Explicit Instantiation - Resample
CLITK_RESAMPLE_IMAGE_LIKE(float, 3, extern);
CLITK_RESAMPLE_IMAGE_LIKE(unsigned char, 3, extern);

// Explicit Instantiation - Crop
CLITK_CROP_LIKE(float, 3, extern);

// Explicit Instantiation - stat
extern template class
itk::LabelStatisticsImageFilter<itk::Image<float, 3>, itk::Image<unsigned char, 3> >;


// Explicit Gaussian filter
#include <itkDiscreteGaussianImageFilter.h>
extern template class
itk::DiscreteGaussianImageFilter< itk::Image<float, 3>, itk::Image<float, 3> >;
