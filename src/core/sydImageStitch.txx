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


//--------------------------------------------------------------------
template<class ImageType>
typename ImageType::Pointer StitchImages(const ImageType * s1,
                                         const ImageType * s2,
                                         double threshold_cumul,
                                         double skip_slices)
{
  // compute bounding box
  typename ImageType::IndexType index1 = s1->GetLargestPossibleRegion().GetIndex();
  typename ImageType::IndexType index2 = s2->GetLargestPossibleRegion().GetIndex();
  typename ImageType::IndexType last1 = index1+s1->GetLargestPossibleRegion().GetSize();
  typename ImageType::IndexType last2 = index2+s2->GetLargestPossibleRegion().GetSize();

  typename ImageType::PointType pstart1, pstart2, pend1, pend2;
  s1->TransformIndexToPhysicalPoint(index1, pstart1);
  s1->TransformIndexToPhysicalPoint(last1, pend1);
  s2->TransformIndexToPhysicalPoint(index2, pstart2);
  s2->TransformIndexToPhysicalPoint(last2, pend2);

  // Check X and Y same size
  bool isCorrect = true;
  for(auto i=0; i<2; i++)  {
    if (pstart1[i] != pstart2[i]) isCorrect = false;
    if (pend1[i] != pend2[i]) isCorrect = false;
  }
  if (!isCorrect) {
    LOG(FATAL) << "Error could not stitch the two images because X/Y size are not equal: "
               << std::endl << "Image1: " << pstart1 << " -> " << pend1
               << std::endl << "Image2: " << pstart2 << " -> " << pend2;
  }

  // Compute the total region
  typename ImageType::PointType pstart3, pend3;
  for(auto i=0; i<3; i++)  {
    if (pstart1[i] > pend1[i]) pstart3[i] = std::max(pstart1[i], pstart2[i]); // reverse order
    if (pstart1[i] < pend1[i]) pstart3[i] = std::min(pstart1[i], pstart2[i]);

    if (pend1[i] > pstart1[i]) pend3[i] = std::max(pend1[i], pend2[i]);
    if (pend1[i] < pstart1[i]) pend3[i] = std::min(pend1[i], pend2[i]); // reverse order
  }

  typename ImageType::IndexType start, end;
  s1->TransformPhysicalPointToIndex(pstart3, start);
  s1->TransformPhysicalPointToIndex(pend3, end);

  typename ImageType::RegionType region;
  typename ImageType::SizeType size;
  for(auto i=0; i<3; i++) size[i] = end[i] - start[i];
  region.SetSize(size);

  typename ImageType::Pointer output = ImageType::New();
  output->CopyInformation(s1);
  output->SetRegions(region);
  typename ImageType::PointType origin;
  for(auto i=0; i<3; i++) {
    if (pstart1[i] > pend1[i]) // reverse
      origin[i] = (pstart1[i] > pstart2[i] ? s1->GetOrigin()[i]:s2->GetOrigin()[i]);
    else
      origin[i] = (pstart1[i] < pstart2[i] ? s1->GetOrigin()[i]:s2->GetOrigin()[i]);
  }
  output->SetOrigin(origin);
  output->Allocate();

  // Resize 2 images like output. interpolation is *needed* because
  // often, s1 (or s2) does not correspond to the whole grid
  typename ImageType::Pointer rs1 = syd::ResampleAndCropImageLike<ImageType>(s1, output, 1, 0);
  typename ImageType::Pointer rs2 = syd::ResampleAndCropImageLike<ImageType>(s2, output, 1, 0);

  // Swap if not correct order
  if (pstart3[2] == pstart1[2]) {
    std::swap(rs1, rs2);
  }

  {
    typedef itk::ImageSliceConstIteratorWithIndex<ImageType> ConstIteratorType;
    typedef itk::ImageSliceIteratorWithIndex<ImageType> IteratorType;
    ConstIteratorType iter1(rs1,rs1->GetLargestPossibleRegion());
    ConstIteratorType iter2(rs2,rs2->GetLargestPossibleRegion());
    IteratorType itero(output,output->GetLargestPossibleRegion());
    iter1.SetFirstDirection(0); iter1.SetSecondDirection(1);
    iter2.SetFirstDirection(0); iter2.SetSecondDirection(1);
    itero.SetFirstDirection(0); itero.SetSecondDirection(1);
    iter1.GoToBegin();
    iter2.GoToBegin();
    itero.GoToBegin();

    std::vector<double> cumul1(rs1->GetLargestPossibleRegion().GetSize()[2]);
    std::vector<double> cumul2(rs2->GetLargestPossibleRegion().GetSize()[2]);

    // Cumulate pixel values along slices (profile)
    int i=0;
    while (!iter1.IsAtEnd()) {
      cumul1[i] = 0.0;
      while (!iter1.IsAtEndOfSlice()) {
        while (!iter1.IsAtEndOfLine()) {
          cumul1[i] += iter1.Get();
          ++iter1;
        }
        iter1.NextLine();
      }
      iter1.NextSlice(); ++i;
    }
    i=0;
    while (!iter2.IsAtEnd()) {
      cumul2[i] = 0.0;
      while (!iter2.IsAtEndOfSlice()) {
        while (!iter2.IsAtEndOfLine()) {
          cumul2[i] += iter2.Get();
          ++iter2;
        }
        iter2.NextLine();
      }
      iter2.NextSlice(); ++i;
    }

    // Find the slice where cumul larger than threshold_cumul
    bool start = false;
    int end=0;
    double t = threshold_cumul;
    for(auto i=0; i<cumul1.size(); i++) {
      if (cumul2[i] > t and start) end = i;
      if (cumul2[i] > t and !start) start = true;
    }

    iter1.GoToBegin();
    iter2.GoToBegin();
    itero.GoToBegin();

    i = 0;
    int skip = skip_slices;
    while (!iter1.IsAtEnd()) {
      while (!iter1.IsAtEndOfSlice()) {
        while (!iter1.IsAtEndOfLine()) {
          if (i>end-skip) itero.Set(iter1.Get());
          else itero.Set(iter2.Get());
          ++iter1; ++iter2; ++itero;
        }
        iter1.NextLine(); iter2.NextLine(); itero.NextLine();
      }
      iter1.NextSlice(); iter2.NextSlice(); itero.NextSlice(); ++i;
    }
  }

  return output;
}
//--------------------------------------------------------------------


