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
#include "sydDicomStructHelper.h"
#include "sydDicomUtils.h"
#include "sydImageUtils.h"

#include <itkSpatialObjectToImageFilter.h>

// --------------------------------------------------------------------
gdcm::SmartPointer<gdcm::SequenceOfItems>
syd::ReadContourSequence(const gdcm::DataSet & dataset,
                         int roi_id)
{
  DDF();
  DD(roi_id);

  // Read the roi
  DD("Read the roi");
  gdcm::Tag troicsq(0x3006,0x0039);
  if (!dataset.FindDataElement(troicsq)) {
    EXCEPTION("Problem locating 0x3006,0x0039 - Is this a valid RT Struct file ?");
  }
  const gdcm::DataElement &roicsq = dataset.GetDataElement(troicsq);
  gdcm::SmartPointer<gdcm::SequenceOfItems> sqi = roicsq.GetValueAsSQ();
  const gdcm::Item & item = sqi->GetItem(roi_id); // Item start at #1
  const gdcm::DataSet & nestedds = item.GetNestedDataSet();

  // Read tag ContourSequence
  //(3006,0040) SQ (Sequence with explicit length #=8)      # 4326, 1 ContourSequence
  DD("Read contour sequence");
  gdcm::Tag tcsq(0x3006,0x0040);
  if (!nestedds.FindDataElement(tcsq)) {
    EXCEPTION("Error while reading RT struct tag (3006,0040) ContourSequence");
  }
  const gdcm::DataElement & csq = nestedds.GetDataElement(tcsq);

  // Read sequence
  DD("Read sequence");
  gdcm::SmartPointer<gdcm::SequenceOfItems> sqi2 = csq.GetValueAsSQ();
  if (!sqi2 || !sqi2->GetNumberOfItems()) {
    EXCEPTION("csq: " << csq << std::endl <<
              "sqi2: " << *sqi2 << std::endl <<
              "Did not find sqi2 or no. items == 0 "
              << sqi2->GetNumberOfItems() << ".");
  }
  DD("done");
  return sqi2;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
const double * syd::ReadContourPoints(gdcm::SmartPointer<gdcm::SequenceOfItems> seq,
                                      int i, // contour index to read
                                      unsigned int & npts)
{
  DDF();
  DD(i);
  const gdcm::Item & item2 = seq->GetItem(i); // Item start at #1
  const gdcm::DataSet& nestedds2 = item2.GetNestedDataSet();
  // (3006,0050) DS [43.57636\65.52504\-10.0\46.043102\62.564945\-10.0\49.126537\60.714... # 398,48 ContourData
  gdcm::Tag tcontourdata(0x3006,0x0050);
  const gdcm::DataElement & contourdata = nestedds2.GetDataElement( tcontourdata );
  gdcm::Attribute<0x3006,0x0050> at;
  at.SetFromDataElement(contourdata);
  const double* pts = at.GetValues();
  DD(at.GetNumberOfValues());
  npts = at.GetNumberOfValues()/3;
  DD(npts);
  DD(pts[0]);
  DD(pts[1]);
  DD(pts[2]);
  // make a copy because 'at' is destroyed
  double * p = new double[at.GetNumberOfValues()];
  std::copy(pts, pts+at.GetNumberOfValues(), p);
  return p; //FIXME as a parameter (vector to resize)
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
int syd::CreatePointIndexList(const double * pts,
                              unsigned int npts,
                              MaskImageType * image,
                              PolygonPointListType & pointList,
                              unsigned int & nb_of_points_outside)
{
  DDF();
  DD(npts);
  pointList.clear();
  typedef itk::Point<double, 3> PointType;
  PointType point;
  MaskImageType::IndexType pixelIndex;
  PolygonPointType p;
  for(auto j = 0; j<npts*3; j+=3) {
    //    DD(j);
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
    p.SetRed(1);
    p.SetBlue(1);
    p.SetGreen(1);
    pointList.push_back(p);

    std::cout << j/3 << " " << point << " " << pixelIndex << std::endl;
  }
  return pixelIndex[2];
  DD("done");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::MergeImages(MaskImageSliceType * slice,
                      MaskImageType * image,
                      int slice_nb)
{
  MaskImageSliceType::PixelType pixelValue =0;
  MaskImageType::IndexType pixelIndex;
  MaskImageSliceType::IndexType sliceIndex;
  int iX = image->GetLargestPossibleRegion().GetSize()[0];
  int iY = image->GetLargestPossibleRegion().GetSize()[1];
  DD(slice_nb);

  if (slice_nb>0) {
    pixelIndex[2] = slice_nb;
    for (int i=0;i<iX;i++)
      for (int j=0;j<iY;j++) { //FIXME SLOW !!!
        pixelIndex[0] = i;
        pixelIndex[1] = j;
        sliceIndex[0] = i;
        sliceIndex[1] = j;

        pixelValue = slice->GetPixel(sliceIndex);

        //Disable hole filling (if required please uncomment the next line (and
        //comment the following line)).

        //if (pixelValue != 0)  finalImage->SetPixel(pixelIndex, pixelValue  );
        image->SetPixel(pixelIndex, pixelValue);//image->GetPixel(pixelIndex) ^ (pixelValue != 0));
      }
  }
  DD("done MergeImages");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertContourSlice(PolygonPointListType & pointList,
                             GroupType * group,
                             MaskImageSliceType * slice,
                             MaskImageType * image,
                             int slice_index)
{
  DDF();
  DD(slice_index);

  // reset 2D slice
  slice->FillBuffer(itk::NumericTraits<MaskPixelType>::Zero);

  // need to create a 2D slice here, put the polygon on it, and insert it back
  // into the 3D volume.
  PolygonType::Pointer polygon = PolygonType::New();
  group->AddSpatialObject(polygon); // add a new polygon group
  polygon->SetPoints(pointList);    // so copy them to a polygon object
  typedef itk::SpatialObjectToImageFilter<GroupType, MaskImageSliceType> SpatialObjectToImageFilterType;
  SpatialObjectToImageFilterType::Pointer imageFilter = SpatialObjectToImageFilterType::New();
  // FIXME --> create filter at each loop iteartion ??
  imageFilter->SetInput(group);
  imageFilter->SetSize(slice->GetLargestPossibleRegion().GetSize());
  imageFilter->Update();
  slice = imageFilter->GetOutput();

  // merge new polygon from temp image into the contour image
  MergeImages(slice, image, slice_index);

  //remove the polygon
  group->RemoveSpatialObject(polygon);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::CreateRoiMaskFromDicomStruct(const gdcm::DataSet & dataset,
                                       int roi_id,
                                       MaskImageType * image)
{
  DDF();
  unsigned int nb_of_points_outside = 0;
  PolygonPointListType pointList;
  GroupType::Pointer group = GroupType::New();

  // Create a temporary slice (not need for spacing, origin etc)
  MaskImageSliceType::Pointer slice = MaskImageSliceType::New();
  MaskImageSliceType::RegionType region;
  MaskImageSliceType::IndexType start;
  MaskImageSliceType::SizeType size;
  start[0] = image->GetLargestPossibleRegion().GetIndex()[0];
  start[1] = image->GetLargestPossibleRegion().GetIndex()[1];
  size[0] = image->GetLargestPossibleRegion().GetSize()[0];
  size[1] = image->GetLargestPossibleRegion().GetSize()[1];
  region.SetSize(size);
  region.SetIndex(start);
  slice->SetRegions(region);
  slice->Allocate();

  // Get the sequence
  auto seq = ReadContourSequence(dataset, roi_id);
  DD(seq->GetNumberOfItems());

  // Loop on the list of contours
  for(auto i=1; i<=seq->GetNumberOfItems(); ++i) { // item start at #1
    DD(i);

    // Retrieve contour points
    unsigned int npts;
    auto pts = ReadContourPoints(seq, i, npts);

    // create list of discrete index from points
    auto slice_index = CreatePointIndexList(pts, npts, image, pointList, nb_of_points_outside);

    // Insert a slice in the image
    InsertContourSlice(pointList, group, slice, image, slice_index);

    // Clean up
    pointList.clear();
  }
  DD(nb_of_points_outside);
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DicomSerie::pointer syd::FindAssociatedDicomSerie(syd::DicomStruct::pointer dicom_struct)
{
  DDF();

  auto filename = dicom_struct->dicom_files[0]->GetAbsolutePath();
  DD(filename);

  auto reader = syd::ReadDicomStructHeader(filename); // will raise exception if not ok
  auto dataset = reader.GetFile().GetDataSet();

  DD("here");

  gdcm::Tag tsssq(0x3006,0x0014);
  if (!dataset.FindDataElement(tsssq)) {
    LOG(FATAL) << "Problem locating XXXX?" << std::endl; /// FIXME bug here
  }
  DD("here");
  auto & sssq = dataset.GetDataElement(tsssq);
  DD("here");
  auto seq = sssq.GetValueAsSQ();
  DD("here");
  // for(auto i = 0; i < seq->GetNumberOfItems(); ++i){
  //   DD(i);
  auto & item = seq->GetItem(0); // Item starts at 1
  DD("here");
  auto & nested_dataset = item.GetNestedDataSet();
  DD("here");
  auto s = syd::GetTagValueAsString<0x20,0x0e>(nested_dataset);
  DD(s);
  // }

  // std::string s =
  //   syd::GetTagValueAsString<0x20,0x0e>(dataset);
  // DD(s);


}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer syd::InsertRoiMaskImageFromDicomStruct(syd::DicomStruct::pointer dicom_struct,
                                                                  syd::RoiType::pointer roi_type,
                                                                  syd::Image::pointer image,
                                                                  std::string roi_name)
{
  DDF();
  DD(dicom_struct);
  DD(image);
  DD(roi_type);
  DD(roi_name);

  // roi_name --> roi id
  int roi_id = 1;
  DD(roi_id);

  // Get dicom dataset
  if (dicom_struct->dicom_files.size() != 1) {
    EXCEPTION("Error while reading datase in DicomStruct file. Expect a single file, found "
              << dicom_struct->dicom_files.size());
  }
  auto filename = dicom_struct->dicom_files[0]->GetAbsolutePath();
  auto reader = syd::ReadDicomStructHeader(filename);
  auto & dataset = reader.GetFile().GetDataSet(); // reader object must not be destroyed to use dataset

  DD("read done");
  gdcm::Tag troicsq(0x3006,0x0039);
  if( !dataset.FindDataElement( troicsq ) )
    {
      std::cout << "Problem locating 0x3006,0x0039 - Is this a valid RT Struct file?" << std::endl;
      LOG(FATAL) << "bug";
    }


  DD("read done");

  // Create empty 3D image
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  // FIXME no need to read image
  auto input_itk_image = syd::ReadImage<ImageType>(image->GetAbsolutePath());
  auto itk_image = syd::CreateImageLike<MaskImageType>(input_itk_image);
  syd::WriteImage<MaskImageType>(itk_image, "bidon_empty.mhd");

  // Create the mask
  syd::CreateRoiMaskFromDicomStruct(dataset, roi_id, itk_image);

  // Create a roi mask image FIXME later
  syd::WriteImage<MaskImageType>(itk_image, "bidon.mhd");

  DD("OK");
  exit(0);

  //  Find point for contours
  //  auto pointList = syd::ReadContourPoints(dataset, roi_id);
  //auto itk_slice = syd::CreateEmptySlice(itk_image);

}
// --------------------------------------------------------------------
