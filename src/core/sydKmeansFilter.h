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

#ifndef SYDKMEANSFILTER_H
#define SYDKMEANSFILTER_H

// itk
#include <itkListSample.h>
#include <itkWeightedCentroidKdTreeGenerator.h>
#include <itkKdTreeBasedKmeansEstimator.h>
#include <itkKdTree.h>

// syd
#include "sydImageUtils.h"
#include "sydNDimPoints.h"

// --------------------------------------------------------------------
namespace syd {

  /// TODO
  class KmeansFilter {

  public:
    /// Constructor.
    KmeansFilter();

    /// images types
    typedef float PixelType;
    typedef itk::Image<PixelType, 3> ImageType;
    typedef itk::Image<PixelType, 4> Image4DType;
    typedef itk::ImageRegionIterator<ImageType> IteratorType;
    typedef itk::ImageRegionIterator<Image4DType> Iterator4DType;

    void SetInput(syd::NDimPoints::pointer p);
    void SetNumberOfClusters(int k);
    void Run();
    syd::NDimPoints::pointer GetCenters() const { return centers; }
    ImageType::Pointer ComputeLabeledImage(syd::NDimPoints::pointer centers,
                                           ImageType::Pointer mask,
                                           Image4DType::Pointer input);

  protected:
    syd::NDimPoints::pointer points;
    int K;
    syd::NDimPoints::pointer centers;

    syd::KmeansFilter::ImageType::Pointer
    AllocateOutputImage(syd::KmeansFilter::Image4DType::Pointer input);

    template<int N>
    void RunWithDim();

  }; // class KmeansFilter

#include "sydKmeansFilter.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
