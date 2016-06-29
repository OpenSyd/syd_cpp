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

#ifndef SYDKMEANSINPUTDATABUILDER_H
#define SYDKMEANSINPUTDATABUILDER_H

// syd
#include "sydImageUtils.h"
#include "sydNDimPoints.h"

// --------------------------------------------------------------------
namespace syd {

  /// TODO
  class KmeansInputDataBuilder {

  public:
    /// Constructor.
    KmeansInputDataBuilder();

    // types
    typedef float PixelType;
    typedef itk::Image<PixelType, 3> ImageType;
    typedef itk::Image<PixelType, 4> Image4DType;
    typedef itk::ImageRegionIterator<ImageType> IteratorType;
    typedef itk::ImageRegionIterator<Image4DType> Iterator4DType;

    void SetMask(ImageType::Pointer m);
    void AddInput(ImageType::Pointer image);
    void AddInput(Image4DType::Pointer image, std::vector<int> indices);

    void BuildInputData();

    syd::NDimPoints & GetInputKmeansData() { return points; }
    int GetNumberOfDimensions() const { return nb_dimensions; }
    Image4DType::Pointer GetInputVectorImage() { return output; }

  protected:
    ImageType::Pointer mask;
    std::vector<ImageType::Pointer> input_images;
    std::vector<Image4DType::Pointer> input_vector_images;
    std::vector<std::vector<int>> input_vector_images_offsets;
    syd::NDimPoints points;
    Image4DType::Pointer output;
    int nb_dimensions;

    void SetValuesFromVectorImage(const std::vector<PixelType*> & iter_vector,
                                  double * v, int & x);
    void AllocateOutputImage(int nb_dimensions);
    template<class T>
    void AllocateOutputImageFromT(int nb_dimensions, typename T::Pointer input);
    void PreProcessing();
    void PostProcessing();
    void InsertVectorImagesAsImages();

  }; // class KmeansInputDataBuilder

#include "sydKmeansInputDataBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
