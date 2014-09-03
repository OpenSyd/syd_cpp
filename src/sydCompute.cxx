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
#include "sydQuery.h"
#include "sydTimeActivityCurve.h"

// itk
#include <itksys/SystemTools.hxx>
#include <itkMultiplyImageFilter.h>

// clitk
#include <clitkCropLikeImageFilter.h>
#include <clitkResampleImageWithOptionsFilter.h>
#include "sydEI.txx"

// gdcm
#include "gdcmFile.h"
#include "gdcmReader.h"

// std
#include <stdio.h>
#include <unistd.h>
#include <algorithm>

// --------------------------------------------------------------------
RoiStudy syd::sydQuery::InsertRoi(Study study, std::string roiname, std::string mhd)
{
  // Check for WholeImage roi
  bool nomask = false;
  if (roiname == "WholeImage") nomask = true;

  // Check mhd exist
  if (!nomask) AbortIfFileNotExist(mDataPath, mhd);

  // Get RoiType from roi name
  RoiType roitype;
  Load<RoiType>(roitype, odb::query<RoiType>::Name == roiname);

  // Create or update RoiStudy from roitype and study
  typedef odb::query<RoiStudy> q;
  std::vector<RoiStudy> roistudies;
  LoadVector<RoiStudy>(roistudies, (q::RoiTypeId == roitype.Id) && (q::StudyId == study.Id));
  if (roistudies.size() > 1) FATAL("Several RoiStudy with RoiTypeId=" << roitype.Id
                                   << " and StudyId=" << study.Id << ". Abort" << std::endl);

  // Create or update
  odb::transaction t (db->begin());
  RoiStudy r;
  if (roistudies.size() == 1) r = roistudies[0];
  else {
    r.StudyId = study.Id;
    r.RoiTypeId = roitype.Id;
    db->persist(r); // Does not exist : create
  }
  r.MHDFilename = mhd;
  db->update(r);
  t.commit();

  // Verbose
  if (GetVerboseFlag()) {
    std::cout << "InsertRoi RoiStudy " << GetById<Patient>(study.PatientId).SynfrizzId << " "
              << study.Number << " " << roiname << " " << mhd ;
    if (roistudies.size() == 1) std::cout << " (updated)";
    else std::cout << " (new)";
    std::cout << std::endl;
  }

  return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
Serie syd::sydQuery::InsertSPECT(Study study, std::string dcm, std::string mhd)
{
  // Check dcm/mhd exist
  syd::AbortIfFileNotExist(mDataPath, dcm);
  syd::AbortIfFileNotExist(mDataPath, mhd);

  // Open dicom
  gdcm::Reader reader;
  reader.SetFileName(std::string(mDataPath+dcm).c_str());
  reader.Read();
  gdcm::File * mFile = &(reader.GetFile());
  const gdcm::DataSet & ds = mFile->GetDataSet();
  gdcm::StringFilter sf;
  sf.SetFile(reader.GetFile());

  // DataSetUID
  std::string DataSetUID;
  syd::ReadTagString(sf, 0x0009, 0x101e, DataSetUID);

  // PixelScale
  double PixelScale;
  syd::ReadTagDouble(sf, 0x0011,0x103b, PixelScale);

  // AcquisitionDate
  std::string AcquisitionDate;
  syd::ReadTagString(sf, 0x008, 0x0022, AcquisitionDate);

  // AcquisitionTime
  std::string AcquisitionTime;
  syd::ReadTagString(sf, 0x0008, 0x0032, AcquisitionTime);

  // ContentDate
  std::string ContentDate;
  syd::ReadTagString(sf, 0x0008, 0x0023, ContentDate);

  // ContentTime
  std::string ContentTime;
  syd::ReadTagString(sf, 0x0008, 0x0033, ContentTime);

  // TableTraverse (From sequence)
  double TableTraverse;
  syd::ReadTagFromSeq<0x0018, 0x1131>(ds, 0x0054,0x0052, TableTraverse);

  // ActualFrameDuration
  double ActualFrameDuration;
  syd::ReadTagFromSeq<0x0018, 0x1242>(ds, 0x0054,0x0052, ActualFrameDuration);

  // YYYY-MM-DD HH:MM
  std::string AcqDate;
  syd::MakeDate(AcquisitionDate, AcquisitionTime, AcqDate);

  // YYYY-MM-DD HH:MM
  std::string ReconstructionDate;
  syd::MakeDate(ContentDate, ContentTime, ReconstructionDate);

  // Check fov
  std::string FOV="unknown";
  // if ((TableTraverse > 1400) && (TableTraverse < 1600)) {
  //   FOV = "fov1";
  // }
  // if ((TableTraverse > 900) && (TableTraverse < 1300)) {
  //   FOV = "fov2";
  // }
  std::string DatasetName;
  syd::ReadTagString(sf, 0x0011, 0x1012, DatasetName);
  std::transform(DatasetName.begin(), DatasetName.end(), DatasetName.begin(), ::tolower);
  if (DatasetName.find("fov1") != std::string::npos) FOV="fov1";
  if (DatasetName.find("fov2") != std::string::npos) FOV="fov2";
  if (DatasetName.find("fov 1") != std::string::npos) FOV="fov1";
  if (DatasetName.find("fov 2") != std::string::npos) FOV="fov2";
  if (FOV == "unknown")
    std::cerr << "WARNING fov unknown, TableTraverse = " << TableTraverse << std::endl;

  /* to add ?
     Counts Accumulated
     (0028,0051) ?? (CS) [ATTN]                                        # 4,1-n Corrected Image
     Energy Window Information Sequence
     (fffe,e000) na (Item with undefined length)
     (0054,0013) ?? (SQ)                                           # u/l,1 Energy Window Range Sequence
     (fffe,e000) na (Item with undefined length)
     (0054,0014) ?? (DS) [153.9 ]                              # 6,1 Energy Window Lower Limit
     (0054,0015) ?? (DS) [188.1 ]                              # 6,1 Energy Window Upper Limit
     (fffe,e00d)
     (fffe,e000) na (Item with undefined length)
     (0054,0014) ?? (DS) [220.5 ]                              # 6,1 Energy Window Lower Limit
     (0054,0015) ?? (DS) [269.5 ]                              # 6,1 Energy Window Upper Limit
     (fffe,e00d)
     (fffe,e0dd)
     (0054,0018) ?? (SH) [In111_EM]                                # 8,1 Energy Window Name
     (0011,100d) ?? (LO) [In111 SC[171 245 and 209] ]                  # 26,1 Radio Nuclide Name
  */

  // Create or update
  typedef odb::query<Serie> q;
  std::vector<Serie> series;
  LoadVector<Serie>(series, (q::DataSetUID == DataSetUID));

  Serie serie;
  serie.StudyId = study.Id; // important : must be set before persist
  odb::transaction t (db->begin());
  bool update = false;
  if (series.size() > 1) FATAL("Two series with same DatasetUID. Abort.");
  if (series.size() == 1) { serie = series[0]; update = true; } // get current serie
  if (series.size() == 0) db->persist(serie); // create

  // Set values
  serie.AcqDate = AcqDate;
  serie.DicomFilename = dcm;
  serie.MHDFilename = mhd;
  serie.ReconstructionDate = ReconstructionDate;
  serie.AcqDurationInSec = syd::toString(ActualFrameDuration);
  serie.FOV = FOV;
  serie.DataSetUID = DataSetUID;
  serie.PixelScale = PixelScale;

  // Update
  db->update(serie);
  t.commit();

  // Verbose
  if (GetVerboseFlag()) {
    std::cout << "InsertSPECT Serie " << GetById<Patient>(study.PatientId).SynfrizzId << " "
              << study.Number << " " << serie.Id << " " << dcm ;
    if (update) std::cout << " (updated)";
    else std::cout << " (new)";
    std::cout << std::endl;
  }

  return serie;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::InsertCT(Study study, std::string mhd)
{
  // Check mhd exist
  syd::AbortIfFileNotExist(mDataPath, mhd);

  // Store value
  odb::transaction t (db->begin());
  study.ReferenceCTFilename = mhd;
  db->update(study);
  t.commit();

  // Verbose
  if (GetVerboseFlag()) {
    std::cout << "InsertCT " << GetById<Patient>(study.PatientId).SynfrizzId << " "
              << study.Number << " " << mhd << std::endl;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeRoiTimeActivity(Study study)
{
  std::vector<RoiStudy> roistudies;
  LoadVector<RoiStudy>(roistudies, odb::query<RoiStudy>::StudyId == study.Id);
  for(auto i=roistudies.begin(); i<roistudies.end(); i++) {
    if (GetVerboseFlag()) {
      std::cout << GetById<Patient>(study.PatientId).SynfrizzId << " "
                << study.Number << " " << GetById<RoiType>(i->RoiTypeId).Name << " ";
    }
    ComputeRoiTimeActivity(*i);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeRoiTimeActivity(RoiStudy roistudy)
{

  // No need to check fov -> select first
  std::vector<Serie> series; // list of 5 or 6 serie
  {
    typedef odb::query<Serie> q;
    LoadVector<Serie>(series, q::StudyId == roistudy.StudyId);// && q::FOV == "fov1");
    if (series.size() == 0) FATAL("Could not find series ? " << std::endl << mCurrentSQLQuery);
  }

  // Check for WholeImage roi
  RoiType roitype;
  Load<RoiType>(roitype, odb::query<RoiType>::Id == roistudy.RoiTypeId);
  bool nomask = false;
  if (roitype.Name == "WholeImage") nomask = true;

  // load roistudy.MHDFilename;
  std::string f(mDataPath+roistudy.MHDFilename);
  MaskImageType::Pointer initialmask;
  MaskImageType::Pointer mask;
  if (!nomask) initialmask = clitk::readImage<MaskImageType>(f);

  // Compute values
  std::vector<std::vector<double> > results;
  for(auto i=series.begin(); i != series.end(); i++) {
    Serie & s = *i;
    ImageType::Pointer spect = clitk::readImage<ImageType>(mDataPath+s.MHDFilename);

    if (nomask) { // create a mask
      mask = MaskImageType::New();
      mask->CopyInformation(spect);
      mask->SetRegions(spect->GetLargestPossibleRegion());
      mask->Allocate();
      mask->FillBuffer(1);
    }
    else { // get mask and resample like the spect
      mask = clitk::ResampleImageLike<MaskImageType>(initialmask, spect, 0, 0);
      //  clitk::writeImage<MaskImageType>(mask, "m.mhd");
    }

    std::vector<double> res;

    typename StatisticsImageFilterType::Pointer statisticsFilter=StatisticsImageFilterType::New();
    statisticsFilter->SetInput(spect);
    statisticsFilter->SetLabelInput(mask);
    statisticsFilter->Update();

    res.clear();
    res.push_back(statisticsFilter->GetCount(1)); // 0 = total counts
    double m = statisticsFilter->GetMean(1);
    // mean by pixel to convert into mean by cc (spacing is in mm, so x0.001)
    double vol = (spect->GetSpacing()[0]*spect->GetSpacing()[1]*spect->GetSpacing()[2]*0.001);
    m = m/vol;
    res.push_back(m); // 1 = mean/vol
    res.push_back(statisticsFilter->GetSigma(1)/vol); // 2 = sigma/vol
    res.push_back(statisticsFilter->GetSum(1));       // 3 = sum
    //    res.push_back(statisticsFilter->GetMaximum(1)/vol);

    {
      // To compute peakValue instead of maxValue, we Gauss filter the image before
      auto gfilter = itk::DiscreteGaussianImageFilter<ImageType, ImageType>::New();
      double * var = new double[3];
      // var = (shrink factor / 2)^2
      var[0] = var[1] = var[2] = 25;//GetGaussianVariance(); FIXME
      gfilter->SetVariance(var);
      gfilter->SetUseImageSpacingOn(); // variance in voxel or mm
      gfilter->SetInput(spect);
      gfilter->Update();
      spect = gfilter->GetOutput();

      statisticsFilter->SetInput(spect);
      statisticsFilter->Update();
      res.push_back(statisticsFilter->GetMaximum(1)/vol); // 4 = max value (after Gaussian)
    }

    results.push_back(res);
  }

  // Insert or update.
  // First load existing series
  std::vector<RoiSerie> roiseries;
  {
    typedef odb::query<RoiSerie> query;
    query q;
    for(auto i=series.begin(); i != series.end(); i++) {
      Serie & s = *i;
      if (i==series.begin()) q = (query::SerieId == s.Id);
      q = q || (query::SerieId == s.Id);
    }
    q = (q) && (query::RoiStudyId == roistudy.Id);
    LoadVector<RoiSerie>(roiseries, q);
  }

  // Insert or update
  odb::transaction t (db->begin());
  struct find_id:std::unary_function<RoiSerie, bool> {
    unsigned long id;
    find_id(unsigned long id):id(id) {}
    bool operator()(RoiSerie const & r) const {
      return r.SerieId == id;
    }
  };
  int n=0;
  int nbOfUpdated=0;
  std::vector<double> means;
  std::vector<double> tot;
  for(auto i=series.begin(); i != series.end(); i++) {
    Serie & s = *i;
    RoiSerie r;
    auto iter = std::find_if(roiseries.begin(), roiseries.end(), find_id(s.Id));
    if (iter != roiseries.end()) {
      r = *iter; // Already exists, we will just update
      nbOfUpdated++;
    }
    else {
      r.SerieId = s.Id;
      r.RoiStudyId = roistudy.Id;
      db->persist(r); // Does not exist : create
    }
    r.MeanActivity  = results[n][1] * s.PixelScale; // mean by cc
    r.StdActivity   = results[n][2] * s.PixelScale; // sigma
    r.TotalActivity = results[n][3] * s.PixelScale; // total in the whole roi
    r.MaxActivity = results[n][4] * s.PixelScale; // max activity
    means.push_back(r.MeanActivity);
    tot.push_back(r.TotalActivity);
    db->update(r);
    ++n;
  }
  t.commit();

  // Verbose
  if (GetVerboseFlag()) {
    std::cout << "Total counts : ";
    //    for(auto i=0; i<means.size(); i++) std::cout << means[i] << " ";
    for(auto i=0; i<tot.size(); i++) std::cout << tot[i] << " ";
    std::cout << " (updated/new = " << nbOfUpdated << "/" << results.size()-nbOfUpdated << ")" << std::endl;
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeRoiInfo(Study study, std::string roiname)
{
  if (roiname == "all" || roiname == "All") {
    std::vector<RoiStudy> roistudies;
    LoadVector<RoiStudy>(roistudies, odb::query<RoiStudy>::StudyId == study.Id);
    for(auto i=roistudies.begin(); i<roistudies.end(); i++)
      ComputeRoiInfo(*i);
  }
  else {
    RoiStudy roistudy = GetRoiStudy(study, roiname);
    ComputeRoiInfo(roistudy);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeRoiInfo(RoiStudy roistudy)
{
  // Get study from roistudy
  Study study;
  Load<Study>(study, odb::query<Study>::Id == roistudy.StudyId);

  // read ct
  std::string f(mDataPath+study.ReferenceCTFilename);
  ImageType::Pointer ct = clitk::readImage<ImageType>(f);

  // Check for WholeImage roi
  RoiType roitype;
  Load<RoiType>(roitype, odb::query<RoiType>::Id == roistudy.RoiTypeId);
  bool nomask = false;
  if (roitype.Name == "WholeImage") nomask = true;

  // read mask
  f = std::string (mDataPath+roistudy.MHDFilename);
  MaskImageType::Pointer mask;
  if (!nomask) mask = clitk::readImage<MaskImageType>(f);
  else {
    mask = MaskImageType::New();
    mask->CopyInformation(ct);
    mask->SetRegions(ct->GetLargestPossibleRegion());
    mask->Allocate();
    mask->FillBuffer(1);
  }

  // crop ct like mask to allow statistic (and gain time because mask is smaller)
  ct = clitk::ResampleImageLike<ImageType>(ct, mask, 0, 1); // O is BG, 1 is linear

  // stat
  typename StatisticsImageFilterType::Pointer statisticsFilter=StatisticsImageFilterType::New();
  statisticsFilter->SetInput(ct);
  statisticsFilter->SetLabelInput(mask);
  statisticsFilter->Update();
  double c = statisticsFilter->GetCount(1);
  double m = statisticsFilter->GetMean(1);

  // Update info
  roistudy.VolumeInCC = c*ct->GetSpacing()[0]*ct->GetSpacing()[1]*ct->GetSpacing()[2]*0.001;
  roistudy.Density = (m/1000+1);
  roistudy.MassInGram = (m/1000+1)*roistudy.VolumeInCC;

  // Store into db
  odb::transaction t (db->begin());
  db->update(roistudy);
  t.commit();

  // verbose
  if (GetVerboseFlag()) {
    std::cout << "Compute info for Roi " << roistudy.Id << " Study " << roistudy.StudyId
              << " vol=" << roistudy.VolumeInCC << " cc  and mass="
              << roistudy.MassInGram << " g" << std::endl;
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeCumulActivityImage(std::vector<Study> studies, int n)
{
  for(auto i=studies.begin(); i<studies.end(); i++)
    ComputeCumulActivityImage(*i, n);
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::sydQuery::ComputeCumulActivityImage(Study study, int nb)
{
  // Get all roiseries
  std::vector<Serie> series;
  GetSortedSeries(study, series);

  // Get times
  std::vector<double> times;
  for(auto i=series.begin(); i<series.end(); i++) times.push_back(i->TimeFromInjectionInHours);

  // open the images
  std::vector<ImageType::Pointer> spect;
  for(auto i=series.begin(); i<series.end(); i++) {
    std::string f = std::string (mDataPath+i->MHDFilename);
    ImageType::Pointer s = clitk::readImage<ImageType>(f);
    spect.push_back(s);

    // needed or not ?
    // auto gfilter = itk::DiscreteGaussianImageFilter<ImageType, ImageType>::New();
    // double * var = new double[3];
    // // var = (shrink factor / 2)^2
    // var[0] = var[1] = var[2] = 25; // 25 = 1CC
    // gfilter->SetVariance(var);
    // gfilter->SetUseImageSpacingOn(); // variance in voxel or mm
    // gfilter->SetInput(s);
    // gfilter->Update();
    // s = gfilter->GetOutput();

  }

  // create output image
  auto output = clitk::NewImageLike<ImageType>(spect[0], true);

  // Debug images
  auto rms_image = clitk::NewImageLike<ImageType>(spect[0], true);
  auto A_image = clitk::NewImageLike<ImageType>(spect[0], true);
  auto l_image = clitk::NewImageLike<ImageType>(spect[0], true);

  // Loop
  itk::ImageRegionIterator<ImageType> iter(output, output->GetLargestPossibleRegion());
  iter.GoToBegin();
  int n = spect.size();
  std::vector<double> activities(n);
  double rms;
  double A;
  double lambda;
  long t=0;
  while (!iter.IsAtEnd()) {
    int peakindex=0;
    double peak = 0;
    for(auto i=0; i<n; i++) {
      activities[i] = spect[i]->GetBufferPointer()[t];
      if (activities[i] > peak) {
        peakindex = i;
        peak = activities[i];
      }
    }
    int m = activities.size() - peakindex;
    m = std::min(m, nb);
    double v = Tac_Integrate(times, activities, m, rms, A, lambda);

    // Some sanity check (WARNING !)
    double minactivity = 500;
    double maxrms=1000;
    double maxlambda = Lambda_Indium*1.5;
    double minlambda = Lambda_Indium*0.5;

    if (isnan(v)) v = -1;
    if (activities[peakindex] < minactivity) v = -2; // dont look at lower activity
    //    if (v<0) v = 0; // no negative
    //if (v>1000000) v = 1000000; // no too high
    //    DD(rms);
    if (rms > maxrms) v = -3;
    if (lambda > maxlambda) v = -4;
    if (lambda < minlambda) v = -5;
    if (isnan(rms)) v = -6;

    // debug image
    rms_image->GetBufferPointer()[t] = rms;
    A_image->GetBufferPointer()[t] = A;
    l_image->GetBufferPointer()[t] = lambda;

    // Set value
    iter.Set(v);
    ++t;
    ++iter;
  }

  // Compute output filename
  std::string f = std::string(mDataPath+series[0].MHDFilename);
  syd::replace(f, "spect1", "cumul");
  if (GetVerboseFlag()) { std::cout << f << std::endl; }
  clitk::writeImage<ImageType>(output, f);

  // Update db
  {
    odb::transaction t (db->begin());
    std::string g = f;
    syd::replace(g, mDataPath, "");
    DD(g);
    study.CumulatedActivityImageFilename = g;
    db->update(study);
    t.commit();
  }


  // debug image
  f = std::string(mDataPath+series[0].MHDFilename);
  syd::replace(f, "spect1", "rms");
  clitk::writeImage<ImageType>(rms_image, f);
  f = std::string(mDataPath+series[0].MHDFilename);
  syd::replace(f, "spect1", "A");
  clitk::writeImage<ImageType>(A_image, f);
  f = std::string(mDataPath+series[0].MHDFilename);
  syd::replace(f, "spect1", "lambda");
  clitk::writeImage<ImageType>(l_image, f);

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeTiming(Study study)
{
  // Find all series for this study and this fov
  typedef odb::query<Serie> q;
  std::vector<Serie> series;
  LoadVector<Serie>(series, (q::StudyId == study.Id));
  if (series.size() == 0) FATAL("No serie found for study " << study.Id << std::endl);

  // Update time
  for(auto i=series.begin(); i<series.end(); i++) {
    ComputeTiming(study, *i);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeTiming(Study study, Serie serie)
{
  // Update time
  odb::transaction t (db->begin());
  double dt = DateDifferenceInHours(serie.AcqDate, study.InjectionDate);
  serie.TimeFromInjectionInHours = dt;
  db->update(serie);
  t.commit();

  // Verbose
  if (GetVerboseFlag()) {
    std::cout << "Timing for study " << study.Id << " serie " << serie.Id
              << " with time : " << dt << " hours" << std::endl;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeRoiCumulActivity(std::vector<RoiStudy> roistudies, int n)
{
  for(auto i=roistudies.begin(); i<roistudies.end(); i++)
    ComputeRoiCumulActivity(*i, n);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeRoiCumulActivity(RoiStudy roistudy, int n)
{
  // Get all objects associated with this roistudy
  Study study = GetById<Study>(roistudy.StudyId);
  Patient patient = GetById<Patient>(study.PatientId);
  RoiType roitype = GetById<RoiType>(roistudy.RoiTypeId);

  // Get all roiseries
  std::vector<RoiSerie> roiseries;
  GetSortedRoiSeries(roistudy, roiseries);

  // Get times / activities values
  std::vector<double> times;
  std::vector<double> activities;
  for(auto i=roiseries.begin(); i<roiseries.end(); i++) {
    double t = GetById<Serie>(i->SerieId).TimeFromInjectionInHours;
    times.push_back(t);
    activities.push_back(i->TotalActivity);
  }

  // Find time peak activity
  int peakindex=0;
  double peak = 0;
  for(auto i=0; i<activities.size(); i++) {
    if (activities[i] > peak) {
      peakindex = i;
      peak = activities[i];
    }
  }

  // m is the number of (final) points used for the exponential fit
  int m = activities.size() - peakindex;
  m = std::min(m, n);
  assert(m>1);

  // Compute integral
  double rms;
  double lambda;
  double A;
  roistudy.CumulatedActivity = Tac_Integrate(times, activities, m, rms, A, lambda);
  roistudy.FitLambda = lambda;
  roistudy.FitA = A;
  roistudy.FitRMS = rms;
  roistudy.FitPeakTime = times[peakindex];

  // Update
  odb::transaction t (db->begin());
  db->update(roistudy);
  t.commit();

  // Verbose
  if (GetVerboseFlag()) {
    std::cout << patient.SynfrizzId << " " << study.Number << " "
              << roitype.Name << " "
              << roistudy.CumulatedActivity << " "
              << roistudy.FitLambda << " "
              << roistudy.FitA << " "
              << roistudy.FitRMS << " "
              << roistudy.FitPeakTime << std::endl;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeRoiFitActivityTest(std::vector<RoiStudy> roistudies, int n)
{
  for(auto i=roistudies.begin(); i<roistudies.end(); i++)
    ComputeRoiFitActivityTest(*i, n);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeRoiFitActivityTest(RoiStudy roistudy, int n)
{
  // Get all objects associated with this roistudy
  Study study = GetById<Study>(roistudy.StudyId);
  Patient patient = GetById<Patient>(study.PatientId);
  RoiType roitype = GetById<RoiType>(roistudy.RoiTypeId);

  // Get all roiseries
  std::vector<RoiSerie> roiseries;
  GetSortedRoiSeries(roistudy, roiseries);

  // Get times / activities values
  std::vector<double> times;
  std::vector<double> activities;
  std::vector<double> variances;
  std::vector<double> std;
  for(auto i=roiseries.begin(); i<roiseries.end(); i++) {
    double t = GetById<Serie>(i->SerieId).TimeFromInjectionInHours;
    times.push_back(t);
    activities.push_back(i->MeanActivity);
    variances.push_back(i->StdActivity*i->StdActivity);
    std.push_back(i->StdActivity);
  }

  // (DEBUG) Dump for gp
  std::ofstream os;
  clitk::openFileForWriting(os, "data.txt");
  os << "0 0 0" << std::endl;
  for(auto i=0; i<times.size(); i++) {
    os << times[i] << " " << activities[i] << " " << std[i] << " " << variances[i] << std::endl;
  }
  os.close();

  // Only n last values
  times.erase(times.begin(), times.end()-n);
  activities.erase(activities.begin(), activities.end()-n);
  variances.erase(variances.begin(), variances.end()-n);

  // Add initial
  // times.insert(times.begin(), 0);
  // activities.insert(activities.begin(), 0);
  // variances.insert(variances.begin(), 0);

  DD("---");
  DDS(times);
  DDS(activities);
  DDS(variances);

  // Trial opti LM
  FitTest(times, activities, variances);

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeRoiPeakCumulActivity(std::vector<RoiStudy> roistudies, double gaussVar)
{
  mPreviousAASpectStudyId = 9999999;
  for(auto i=roistudies.begin(); i<roistudies.end(); i++)
    ComputeRoiPeakCumulActivity(*i, gaussVar);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeRoiPeakCumulActivity(RoiStudy roistudy, double gaussVar)
{
  DD(roistudy);

  // Load the aaSpect image for this study (or retrive in cache)
  Study study = GetById<Study>(roistudy.StudyId);
  ImageType::Pointer aaSpect;
  if (mPreviousAASpectStudyId == study.Id) {
    aaSpect = mCachedAASpect;
  }
  else {
    std::string f(mDataPath+study.CumulatedActivityImageFilename);
    DD(f);
    aaSpect = clitk::readImage<ImageType>(f);

    // To compute peakValue instead of maxValue, we Gauss filter the image before
    auto gfilter = itk::DiscreteGaussianImageFilter<ImageType, ImageType>::New();
    double * var = new double[3];
    // var = (shrink factor / 2)^2
    var[0] = var[1] = var[2] = gaussVar;
    gfilter->SetVariance(var);
    gfilter->SetUseImageSpacingOn(); // variance in voxel or mm
    gfilter->SetInput(aaSpect);
    gfilter->Update();
    aaSpect = gfilter->GetOutput();

    // debug
    clitk::writeImage<ImageType>(aaSpect, "smooth.mhd");

    // Put in 'cache'
    mPreviousAASpectStudyId = study.Id;
    mCachedAASpect = aaSpect;
  }

  // Load the current roi mask
  MaskImageType::Pointer initialmask;
  MaskImageType::Pointer mask;
  GetResampledMask(roistudy, aaSpect, initialmask, mask);

  // Debug
  clitk::writeImage<MaskImageType>(mask, "mm.mhd");

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
  DD(maxValue);
  DD(maxIndex);
  typename ImageType::PointType p;
  aaSpect->TransformIndexToPhysicalPoint(maxIndex, p);
  DD(p);

  // Change mask for a sphere centered on this max
  double volume = 3000.0;
  double radius = pow((volume/(4.0/3.0*M_PI)), 1.0/3.0);
  DD(radius);
  {
    itk::ImageRegionIteratorWithIndex<MaskImageType> iter(initialmask, initialmask->GetLargestPossibleRegion());
    iter.GoToBegin();
    typename ImageType::PointType center;
    mask->TransformIndexToPhysicalPoint(maxIndex, center);
    typename ImageType::PointType p;
    while (!iter.IsAtEnd()) {
      initialmask->TransformIndexToPhysicalPoint(iter.GetIndex(), p);
      double d = p.EuclideanDistanceTo(center);
      if (d < radius) iter.Set(1);
      else iter.Set(0);
      ++iter;
    }
  }
  clitk::writeImage<MaskImageType>(initialmask, "roipeak.mhd");
  MaskImageType::Pointer a = clitk::ResampleImageLike<MaskImageType>(initialmask, aaSpect, 0, 0); // O is BG, 0 is NN interpolation
  clitk::writeImage<MaskImageType>(a, "roipeak-resampled.mhd");

  // Store the new mask as a new roi ?
  //  FIXME
  /// Compute output filename
  std::string f = std::string(mDataPath+roistudy.MHDFilename);
  syd::replace(f, ".mhd", "-Peak.mhd");
  if (GetVerboseFlag()) { std::cout << f << std::endl; }
  clitk::writeImage<MaskImageType>(initialmask, f);

  // Update db
  {
    odb::transaction t (db->begin());
    std::string f = std::string(roistudy.MHDFilename);
    syd::replace(f, ".mhd", "-Peak.mhd");
    roistudy.PeakMHDFilename = f;
    db->update(roistudy);
    t.commit();
  }

  // debug image


  // Verbose
  if (GetVerboseFlag()) {
    Study study = GetById<Study>(roistudy.StudyId);
    Patient patient = GetById<Patient>(study.PatientId);
    RoiType roitype = GetById<RoiType>(roistudy.RoiTypeId);
    std::cout << patient.SynfrizzId << " " << study.Number << " "
              << roitype.Name << " "
              << roistudy.CumulatedActivity << " "
              << roistudy.PeakCumulatedActivityConcentration << " "
              << roistudy.FitLambda << " "
              << roistudy.FitA << " "
              << roistudy.FitRMS << " "
              << roistudy.FitPeakTime << std::endl;
  }

}
// --------------------------------------------------------------------
