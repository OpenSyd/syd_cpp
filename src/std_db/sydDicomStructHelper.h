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

#ifndef SYDDICOMSTRUCTHELPER_H
#define SYDDICOMSTRUCTHELPER_H

// syd
#include "sydDicomStruct.h"
#include "sydDicomSerie.h"
#include "sydRoiMaskImage.h"
#include "sydDicomUtils.h"

// itk
#include <itkPolygonSpatialObject.h>
#include <itkGroupSpatialObject.h>

// --------------------------------------------------------------------
namespace syd {

  /// Search for the dicom associated with the struct
  syd::DicomSerie::pointer FindAssociatedDicomSerie(syd::DicomStruct::pointer s);

  /// Create and insert a mask from a dicom struct
  syd::RoiMaskImage::pointer InsertRoiMaskImageFromDicomStruct(syd::DicomStruct::pointer dicom_struct,
                                                               syd::RoiType::pointer roitype,
                                                               syd::Image::pointer image,
                                                               std::string roi_name);

  // FIXME --> put elsewhere (not dep db)
  typedef itk::PolygonSpatialObject<2> PolygonType;
  typedef itk::SpatialObjectPoint<2> PolygonPointType;
  typedef itk::GroupSpatialObject<2> GroupType;
  typedef PolygonType::PointListType PolygonPointListType;
  typedef unsigned char MaskPixelType;
  typedef itk::Image<MaskPixelType, 2> MaskImageSliceType;
  typedef itk::Image<MaskPixelType, 3> MaskImageType;

  void CreateRoiMaskFromDicomStruct(const gdcm::DataSet & dataset,
                                    int roi_id,
                                    MaskImageType * itk_image);

  gdcm::SmartPointer<gdcm::SequenceOfItems> 
    ReadContourSequence(const gdcm::DataSet & dataset,
                        int roi_id);
  const double * ReadContourPoints(gdcm::SmartPointer<gdcm::SequenceOfItems> seq,
                                   int i,
                                   unsigned int & npts);
  int CreatePointIndexList(const double * pts,
                           unsigned int npts,
                           MaskImageType * image,
                           PolygonPointListType & pointList,
                           unsigned int & nb_of_points_outside);

  void InsertContourSlice(PolygonPointListType & pointList,
                          GroupType * group,
                          MaskImageSliceType * temp2Dimage,
                          MaskImageType * image,
                          int slice_index);

  void MergeImages(MaskImageSliceType * tempSlice,
                   MaskImageType * finalImage,
                   int iRequiredSlice);
}
// --------------------------------------------------------------------

#endif
