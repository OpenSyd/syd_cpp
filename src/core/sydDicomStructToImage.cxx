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

// syd
#include "sydDicomStructToImage.h"

#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <thread>

// --------------------------------------------------------------------
void syd::DicomStructToImageBuilder::
ConvertRoiToImage(const gdcm::DataSet & dataset, int roi_id, MaskImageType * image)
{
  unsigned int nb_of_points_outside = 0;
  PolygonPointListType pointList;
  GroupType::Pointer group = GroupType::New();

  // Create a temporary slice (not need for spacing, origin etc)
  MaskImageSliceType::Pointer slice = MaskImageSliceType::New();
  MaskImageSliceType::RegionType region;
  MaskImageSliceType::IndexType start;
  MaskImageSliceType::SizeType size;
  MaskImageSliceType::PointType origin;
  MaskImageSliceType::SpacingType spacing;
  start[0] = image->GetLargestPossibleRegion().GetIndex()[0];
  start[1] = image->GetLargestPossibleRegion().GetIndex()[1];
  size[0] = image->GetLargestPossibleRegion().GetSize()[0];
  size[1] = image->GetLargestPossibleRegion().GetSize()[1];
  origin[0] = image->GetOrigin()[0];
  origin[1] = image->GetOrigin()[1];
  spacing[0] = image->GetSpacing()[0];
  spacing[1] = image->GetSpacing()[1];
  region.SetSize(size);
  region.SetIndex(start);
  slice->SetOrigin(origin);
  slice->SetRegions(region);
  slice->SetSpacing(spacing);
  slice->Allocate();

  // Get the sequence
  auto seq = ReadContourSequence(dataset, roi_id);

  // Common for all call to InsertSliceFromContour
  PolygonType::Pointer polygon = PolygonType::New();
  SpatialObjectToImageFilterType::Pointer imageFilter = SpatialObjectToImageFilterType::New();

  // Loop on the list of contours
  for(auto i=1; i<=seq->GetNumberOfItems(); ++i) { // item start at #1

    // Retrieve contour points
    unsigned int npts;
    auto pts = ReadContourPoints(seq, i, npts);

    // create list of discrete index from points
    auto slice_index = ConvertContourPointToIndex(pts, npts, image, pointList, nb_of_points_outside);
    delete pts;

    // Insert a slice in the image
    InsertSliceFromContour(polygon, imageFilter, pointList, group, slice, image, slice_index);

    // Clean up
    pointList.clear();
  }
  //  DD(nb_of_points_outside); // not use yet
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
gdcm::SmartPointer<gdcm::SequenceOfItems>
syd::DicomStructToImageBuilder::
ReadContourSequence(const gdcm::DataSet & dataset,
                    int roi_id)
{
  // Read the roi
  gdcm::Tag troicsq(0x3006,0x0039);
  if (!dataset.FindDataElement(troicsq)) {
    EXCEPTION("Problem locating 0x3006,0x0039 - Is this a valid RT Struct file ?");
  }
  const gdcm::DataElement &roicsq = dataset.GetDataElement(troicsq);
  gdcm::SmartPointer<gdcm::SequenceOfItems> sqi = roicsq.GetValueAsSQ();
  const gdcm::Item & item = sqi->GetItem(roi_id); // Item start at #1
  const gdcm::DataSet & nestedds = item.GetNestedDataSet();

  // Read tag ContourSequence
  //(3006,0040) SQ (Sequence with explicit length #=8) # 4326, 1 ContourSequence
  gdcm::Tag tcsq(0x3006,0x0040);
  if (!nestedds.FindDataElement(tcsq)) {
    EXCEPTION("Error while reading RT struct tag (3006,0040) ContourSequence");
  }
  const gdcm::DataElement & csq = nestedds.GetDataElement(tcsq);

  // Read sequence
  gdcm::SmartPointer<gdcm::SequenceOfItems> sqi2 = csq.GetValueAsSQ();
  if (!sqi2 || !sqi2->GetNumberOfItems()) {
    EXCEPTION("csq: " << csq << std::endl <<
              "sqi2: " << *sqi2 << std::endl <<
              "Did not find sqi2 or no. items == 0 "
              << sqi2->GetNumberOfItems() << ".");
  }
  return sqi2;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double * syd::DicomStructToImageBuilder::
ReadContourPoints(gdcm::SmartPointer<gdcm::SequenceOfItems> seq,
                  int i, // contour index to read
                  unsigned int & npts)
{
  const gdcm::Item & item2 = seq->GetItem(i); // Item start at #1
  const gdcm::DataSet & nestedds2 = item2.GetNestedDataSet();
  // (3006,0050) DS [43.57636\65.52504\-10.0\46.043102\62.564945\-10.0\49.126537\60.714... # 398,48 ContourData
  gdcm::Tag tcontourdata(0x3006,0x0050);
  const gdcm::DataElement & contourdata = nestedds2.GetDataElement( tcontourdata );
  gdcm::Attribute<0x3006,0x0050> at;
  at.SetFromDataElement(contourdata);
  const double* pts = at.GetValues();
  npts = at.GetNumberOfValues()/3;
  // make a copy because 'at' is destroyed
  double * p = new double[at.GetNumberOfValues()];
  std::copy(pts, pts+at.GetNumberOfValues(), p);
  return p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::DicomStructToImageBuilder::
ConvertContourPointToIndex(const double * pts,
                           unsigned int npts,
                           MaskImageType * image,
                           PolygonPointListType & pointList,
                           unsigned int & nb_of_points_outside)
{
  pointList.clear();
  typedef itk::Point<double, 3> PointType;
  PointType point;
  MaskImageType::IndexType pixelIndex;
  PolygonPointType p;
  for(auto j = 0; j<npts*3; j+=3) {
    point[0] = pts[j+0];
    point[1] = pts[j+1];
    point[2] = pts[j+2];
    //transform points to image coordinates
    if (!(image->TransformPhysicalPointToIndex(point, pixelIndex))) {
      //Are there points outside the image boundary. This may occur with
      //automatically segmented objects such as benches or external body
      //outlines?
      nb_of_points_outside++;
    }
    p.SetPosition(pixelIndex[0], pixelIndex[1], pixelIndex[2]);
    p.SetColor(1,1,1);
    pointList.push_back(p);
  }
  return pixelIndex[2];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomStructToImageBuilder::
MergeSliceInImage(MaskImageSliceType * slice,
                  MaskImageType * image,
                  int slice_nb)
{
  MaskImageSliceType::PixelType pixelValue =0;
  MaskImageType::IndexType pixelIndex;
  MaskImageSliceType::IndexType sliceIndex;
  int iX = image->GetLargestPossibleRegion().GetSize()[0];
  int iY = image->GetLargestPossibleRegion().GetSize()[1];
  if (slice_nb <=0) return;
  pixelIndex[2] = slice_nb;
  auto pslice = slice->GetBufferPointer();
  for (int i=0;i<iY;i++)
    for (int j=0;j<iX;j++) {
      pixelIndex[0] = j;
      pixelIndex[1] = i;
      pixelValue = *pslice;
      auto v = image->GetPixel(pixelIndex);
      // Add the current contour because another contour may already be on the same slice
      image->SetPixel(pixelIndex, v+pixelValue);
      ++pslice;
    }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomStructToImageBuilder::
InsertSliceFromContour(PolygonType * polygon,
                       SpatialObjectToImageFilterType * imageFilter,
                       PolygonPointListType & pointList,
                       GroupType * group,
                       MaskImageSliceType * slice,
                       MaskImageType * image,
                       int slice_index)
{
  // reset 2D slice
  slice->FillBuffer(itk::NumericTraits<MaskPixelType>::Zero);

  /// need to create a 2D slice here, put the polygon on it, and insert it back
  /// into the 3D volume. This is the slowest part of the whole process
  group->AddSpatialObject(polygon); // add a new polygon group
  polygon->SetPoints(pointList);    // so copy them to a polygon object
  imageFilter->SetInput(group);
  imageFilter->SetSize(slice->GetLargestPossibleRegion().GetSize());
  imageFilter->Update();
  slice = imageFilter->GetOutput();

  // merge new polygon from temp image into the contour image
  MergeSliceInImage(slice, image, slice_index);

  //remove the polygon
  group->RemoveSpatialObject(polygon);
}
// --------------------------------------------------------------------
