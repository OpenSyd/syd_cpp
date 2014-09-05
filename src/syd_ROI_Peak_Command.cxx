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
#include "syd_ROI_Peak_Command.h"

// clitk
#include <clitkResampleImageWithOptionsFilter.h>
#include <clitkCropLikeImageFilter.h>

// itk
#include <itkDiscreteGaussianImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>

// --------------------------------------------------------------------
syd::ROI_Peak_Command::
ROI_Peak_Command(sydQuery & _db):Command(_db)
{
  m_Gaussian_Variance = 25; // 1 CC is gauss=25
  m_Peak_Volume_In_CC = 3;
  m_PreviousAASpectStudyId = 0;
  m_CachedAASpect = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ROI_Peak_Command::
SetArgs(char ** inputs, int n)
{
  // FIXME check nb of args
  assert(n==4);

  // Get all roistudies (patient / study=all / roi)
  db.GetRoiStudies(inputs[0], "all", inputs[1], roistudies);

  // Get parameters
  m_Gaussian_Variance = atof(inputs[2]);
  assert(m_Gaussian_Variance > 0);
  m_Peak_Volume_In_CC = atof(inputs[3]);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ROI_Peak_Command::
Run()
{
  for(auto i=roistudies.begin(); i<roistudies.end(); i++) {
    // Check if the region is not already a peak
    RoiType r = db.GetById<RoiType>(i->RoiTypeId);
    if (r.Name.find("Peak") == std::string::npos) Run(*i);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ROI_Peak_Command::
Run(RoiStudy roistudy)
{
  // Load the aaSpect image for this study (or retrive in cache)
  Study study = db.GetById<Study>(roistudy.StudyId);
  ImageType::Pointer aaSpect;
  assert(study.Id != 0);
  if (m_PreviousAASpectStudyId == study.Id) {
    aaSpect = m_CachedAASpect;
  }
  else {
    if (study.CumulatedActivityImageFilename == "") {
      FATAL("Compute CumulatedActivityImageFilename before");
    }
    aaSpect = db.Read_Image<ImageType>(study.CumulatedActivityImageFilename);
    //aaSpect = clitk::readImage<ImageType>(f);

    // To compute peakValue instead of maxValue, we Gauss filter the image before
    auto gfilter = itk::DiscreteGaussianImageFilter<ImageType, ImageType>::New();
    double * var = new double[3];
    // var = (shrink factor / 2)^2
    var[0] = var[1] = var[2] = m_Gaussian_Variance;
    gfilter->SetVariance(var);
    gfilter->SetUseImageSpacingOn(); // variance in voxel or mm
    gfilter->SetInput(aaSpect);
    gfilter->Update();
    aaSpect = gfilter->GetOutput();

    // debug
    //clitk::writeImage<ImageType>(aaSpect, "smooth.mhd");

    // Put in 'cache'
    m_PreviousAASpectStudyId = study.Id;
    m_CachedAASpect = aaSpect;
  }

  // Load the current roi mask
  MaskImageType::Pointer initialmask;
  MaskImageType::Pointer mask;
  db.Get_Resampled_Mask(roistudy, aaSpect, initialmask, mask);

  // Debug
  //clitk::writeImage<MaskImageType>(mask, "mm.mhd");

  // Find the max pixel in this ROI
  double maxValue = 0;
  typename ImageType::IndexType maxIndex;
  {
    itk::ImageRegionIteratorWithIndex<ImageType> iter(aaSpect, aaSpect->GetLargestPossibleRegion());
    iter.GoToBegin();
    MaskPixelType * m = mask->GetBufferPointer();
    while (!iter.IsAtEnd()) {
      if (*m != 0) { // inside the mask
        if (iter.Get() > maxValue) {
          maxIndex = iter.GetIndex();
          maxValue = iter.Get();
        }
      }
      ++m;
      ++iter;
    }
  }
  typename ImageType::PointType p;
  aaSpect->TransformIndexToPhysicalPoint(maxIndex, p);

  // Get center
  typename ImageType::PointType center;
  mask->TransformIndexToPhysicalPoint(maxIndex, center);

  // Create mask for a sphere centered on this max
  double volume = m_Peak_Volume_In_CC * 1000;
  double radius = pow((volume/(4.0/3.0*M_PI)), 1.0/3.0);

  MaskImageType::Pointer peakmask = MaskImageType::New();
  peakmask->CopyInformation(initialmask);
  MaskImageType::RegionType region;
  MaskImageType::SizeType size;
  MaskImageType::PointType origin;
  for(auto i=0; i<3; i++) {
    size[i] = ((radius*2.0) / initialmask->GetSpacing()[i])+1;
    origin[i] = center[i] - size[i]*initialmask->GetSpacing()[i]/2.0 + initialmask->GetSpacing()[i]/2.0;
  }
  region.SetSize(size);
  peakmask->SetRegions(region);
  peakmask->SetOrigin(origin);
  peakmask->Allocate();

  {
    itk::ImageRegionIteratorWithIndex<MaskImageType> iter(peakmask, peakmask->GetLargestPossibleRegion());
    iter.GoToBegin();
    typename ImageType::PointType p;
    while (!iter.IsAtEnd()) {
      peakmask->TransformIndexToPhysicalPoint(iter.GetIndex(), p);
      double d = p.EuclideanDistanceTo(center);
      if (d < radius) iter.Set(1);
      else iter.Set(0);
      ++iter;
    }
  }

  //clitk::writeImage<MaskImageType>(initialmask, "roipeak.mhd");
  MaskImageType::Pointer a = clitk::ResampleImageLike<MaskImageType>(peakmask, aaSpect, 0, 0); // O is BG, 0 is NN interpolation
  //clitk::writeImage<MaskImageType>(a, "roipeak-resampled.mhd");

  // Store the new mask as a new roi
  std::string roiname = db.GetById<RoiType>(roistudy.RoiTypeId).Name;
  roiname = roiname+"-Peak";

  // Get or create the new roitype name
  RoiType peakroitype;
  if (!db.LoadFirstIfExist<RoiType>(peakroitype, odb::query<RoiType>::Name == roiname)) {
    // Create a new roi type
    peakroitype.Name = roiname;
    peakroitype.Description = "Computed";
    db.Insert<RoiType>(peakroitype);
  }
  else {
    //DD("already exist");
  }

  // Write the image (in the db right folder)
  std::string filename = roistudy.MHDFilename;
  syd::replace(filename, ".mhd", "-Peak.mhd");
  db.Write_Image<MaskImageType>(peakmask, filename);

  // Insert into db
  db.SetVerboseFlag(true);
  RoiStudy peakroistudy = db.InsertRoi(study, roiname, filename);
  db.ComputeRoiInfo(peakroistudy);
  db.ComputeRoiTimeActivity(peakroistudy); // create the n roiserie
  db.SetVerboseFlag(false);

  // Verbose
  if (GetVerboseFlag()) {
    Study study = db.GetById<Study>(roistudy.StudyId);
    Patient patient = db.GetById<Patient>(study.PatientId);
    RoiType roitype = db.GetById<RoiType>(roistudy.RoiTypeId);
    std::cout << patient.SynfrizzId << " " << study.Number << " "
              << roitype.Name << " "
              << peakroitype.Name << " "
              << std::endl;
  }

}
// --------------------------------------------------------------------
