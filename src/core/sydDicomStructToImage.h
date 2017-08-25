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

#ifndef SYDDICOMSTRUCTTOIMAGE_H
#define SYDDICOMSTRUCTTOIMAGE_H

// syd
#include "sydImageUtils.h"

// itk
#include <itkPolygonSpatialObject.h>
#include <itkGroupSpatialObject.h>
#include <itkSpatialObjectToImageFilter.h>

// --------------------------------------------------------------------
namespace syd {

  /// FIXME
  class DicomStructToImageBuilder {
  public:
    typedef unsigned char MaskPixelType;
    typedef itk::Image<MaskPixelType, 2> MaskImageSliceType;
    typedef itk::Image<MaskPixelType, 3> MaskImageType;

    void ConvertRoiToImage(const gdcm::DataSet & dataset, int roi_id, MaskImageType * itk_image);

  protected:
    typedef itk::PolygonSpatialObject<2> PolygonType;
    typedef itk::SpatialObjectPoint<2> PolygonPointType;
    typedef itk::GroupSpatialObject<2> GroupType;
    typedef PolygonType::PointListType PolygonPointListType;
    typedef itk::SpatialObjectToImageFilter<GroupType, MaskImageSliceType> SpatialObjectToImageFilterType;

    /// Read dicom contour seq of the given roi
    gdcm::SmartPointer<gdcm::SequenceOfItems> ReadContourSequence(const gdcm::DataSet & dataset, int roi_id);

    /// Read the points from a dicom contour
    double * ReadContourPoints(gdcm::SmartPointer<gdcm::SequenceOfItems> seq, int i, unsigned int & npts);

    /// Create the list of index; return the slice nb
    int ConvertContourPointToIndex(const double * pts,
                                   unsigned int npts,
                                   MaskImageType * image,
                                   PolygonPointListType & pointList,
                                   unsigned int & nb_of_points_outside);

    /// Create a slice from the contour
    void InsertSliceFromContour(PolygonType * polygon,
                                SpatialObjectToImageFilterType * imageFilter,
                                PolygonPointListType & pointList,
                                GroupType * group,
                                MaskImageSliceType * temp2Dimage,
                                MaskImageType * image,
                                int slice_index);

    /// Merge
    void MergeSliceInImage(MaskImageSliceType * slice, MaskImageType * image, int slice_index);

  };
} // end namespace
// --------------------------------------------------------------------

#endif
