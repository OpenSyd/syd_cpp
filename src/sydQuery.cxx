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
// http://stackoverflow.com/questions/1607368/sql-query-logging-for-sqlite
void trace_callback( void* udp, const char* sql ) {
  syd::sydQuery * q = static_cast<syd::sydQuery*>(udp);
  //  printf("{SQL} [%s]\n", sql);
  q->TraceCallback(sql);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::sydQuery::sydQuery()
{
  SetVerboseFlag(false);
  SetVerboseDBFlag(false);
  SetVerboseQueryFlag(false);
  SetGaussianVariance(1.0);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::sydQuery::~sydQuery()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::OpenDatabase()
{
  // Get DB filename
  char * bdb = getenv ("SYD_DB");
  if (bdb == NULL) FATAL(std::endl << " please set SYD_DB environment variable." << std::endl);
  mDatabaseFilename = std::string(bdb);

  // Open the DB
  try {
    db = new odb::sqlite::database(mDatabaseFilename);
    odb::connection_ptr c(db->connection());
    c->execute("PRAGMA foreign_keys=ON;");
  }
  catch (const odb::exception& e) {
    FATAL(std::endl << "Cannot open db '" << mDatabaseFilename << "' : " << e.what() << std::endl);
  }

  if (GetVerboseDBFlag()) { std::cout << "Opening '" << mDatabaseFilename << "' database." << std::endl; }

  // Get Database folder
  char * b =getenv ("SYD_IMAGE_FOLDER");
  if (b == NULL) FATAL(std::endl << " please set SYD_IMAGE_FOLDER environment variable." << std::endl);
  mDataPath = std::string(b)+"/";
  AbortIfFileNotExist(mDataPath);

  if (GetVerboseDBFlag()) { std::cout << "Data folder is '" << mDataPath << "'." << std::endl; }

  // Install tracer
  odb::sqlite::connection_ptr c (db->connection ());
  sqlite3* handle (c->handle ());
  sqlite3_trace (handle, trace_callback, this);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::TraceCallback(const char* sql)
{
  std::string s = std::string(sql);
  if (s == "COMMIT") return;
  if (s == "BEGIN") return;
  if (GetVerboseQueryFlag()) std::cout << s << std::endl;
  mCurrentSQLQuery=s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
Study syd::sydQuery::GetStudy(char ** inputs)
{
  unsigned long SynfrizzId = atoi(inputs[0]);
  unsigned long StudyNb    = atoi(inputs[1]);
  return GetStudy(SynfrizzId, StudyNb);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
Study syd::sydQuery::GetStudy(unsigned long SynfrizzId, unsigned long StudyNb)
{
  // Get Patient from synfrizz id
  Patient patient;
  Load<Patient>(patient, odb::query<Patient>::SynfrizzId == SynfrizzId);

  // Get study from patient
  std::vector<Study> studies;
  LoadVector<Study>(studies, odb::query<Study>::PatientId == patient.Id);

  // Check StudyNb
  if (studies.size() == 0) FATAL("No study with patient " << SynfrizzId << ". Abort." << std::endl);
  int found = -1;
  for(auto i=studies.begin(); i<studies.end(); i++) {
    if (i->Number == StudyNb) return *i;
  }
  FATAL("No StudyNb = " << StudyNb << " for patient " << SynfrizzId << ". Abort." << std::endl);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
RoiStudy syd::sydQuery::GetRoiStudy(Study study, std::string roiname)
{
  // Get RoiType from roi name
  RoiType roitype;
  Load<RoiType>(roitype, odb::query<RoiType>::Name == roiname);

  // Get RoiStudy from roitype and study
  RoiStudy roistudy;
  typedef odb::query<RoiStudy> q;
  Load<RoiStudy>(roistudy, q::StudyId == study.Id && q::RoiTypeId == roitype.Id);

  return roistudy;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::GetListOfPatients(std::string SynfrizzId, std::vector<unsigned long> & ids)
{
  ids.resize(0);
  if (SynfrizzId == "all") {
    typedef odb::query<Patient> q;
    std::vector<Patient> p;
    LoadVector<Patient>(p, q::SynfrizzId != 0);
    for(auto i=p.begin(); i<p.end(); i++)
      ids.push_back(i->SynfrizzId);
  }
  else ids.push_back(atoi(SynfrizzId.c_str()));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::GetStudies(std::string patients_arg, std::string studies_arg,
                                  std::vector<Study> & studies)
{
  std::vector<Patient> patients;
  typedef odb::query<Patient> q;

  // All patients
  if (patients_arg == "all") LoadVector<Patient>(patients, q::SynfrizzId != 0);
  else {
    if (patients_arg.find(" ") != std::string::npos) { // space exist, so consider multiple Ids
      char *s = strtok((char *)patients_arg.c_str(), " "); // cast char * is needed to prevent warning. Warning : string modified
      while (s) {
        Patient p;
        unsigned long id = atoi(s);
        if (id != 0) { // do not consider id==0 or atoi error
          Load<Patient>(p, q::SynfrizzId == id);
          patients.push_back(p);
        }
        else {
          std::cerr << "Warning patient '" << s << "' is ignored." << std::endl;
        }
        s = strtok(NULL, " ");
      }
    }
    else {
      Patient p;
      Load<Patient>(p, q::SynfrizzId == atoi(patients_arg.c_str()));
      patients.push_back(p);
    }
  }

  // Get studies for thoses patients
  studies.clear();
  for(auto i=patients.begin(); i<patients.end(); i++)
    GetStudies(i->Id, studies_arg, studies);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::GetStudies(unsigned long patientId, std::string studies_arg,
                               std::vector<Study> & studies)
{
  typedef odb::query<Study> q;
  if (studies_arg == "all") LoadVector<Study>(studies, (q::PatientId == patientId));
  else {
    FATAL("Sorry only studies = 'all' allow yet");
    //LoadVector<Study>(studies, (q::Id == atoi(studies_arg.c_str()))); // only a single study here
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::GetRoiTypes(std::string rois_arg, std::vector<RoiType> & roitypes)
{
  typedef odb::query<RoiType> q;
  if (rois_arg == "all") LoadVector<RoiType>(roitypes, q::Name != ""); // all
  else {
    if (rois_arg.find(" ") != std::string::npos) { // space exist, so consider multiple Ids
      char *s = strtok((char *)rois_arg.c_str(), " "); // cast char * is needed to prevent warning. Warning : string modified
      while (s) {
        printf ("Token: %s\n", s);
        RoiType p;
        bool b = LoadFirstIfExist<RoiType>(p, q::Name == s);
        if (b) roitypes.push_back(p);
        s = strtok(NULL, " ");
      }
    }
    else {
      LoadVector<RoiType>(roitypes, q::Name.like(rois_arg));
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::GetRoiStudies(std::string patients_arg, std::string studies_arg, std::string rois_arg,
                                  std::vector<RoiStudy> & roistudies)
{
  roistudies.clear();
  std::vector<Study> studies;
  GetStudies(patients_arg, studies_arg, studies);
  std::vector<RoiType> roitypes;
  GetRoiTypes(rois_arg, roitypes);

  // Check
  if (roitypes.size() == 0) {
    std::cout << "No roi selected, nothing todo" << std::endl;
  }

  if (studies.size() == 0) {
    std::cout << "No studies selected, nothing todo" << std::endl;
  }

  for(auto j=studies.begin(); j<studies.end(); j++) {
    for(auto i=roitypes.begin(); i<roitypes.end(); i++) {
      RoiStudy roistudy;
      typedef odb::query<RoiStudy> q;
      bool b = LoadFirstIfExist<RoiStudy>(roistudy, q::StudyId == j->Id && q::RoiTypeId == i->Id);
      if (b) roistudies.push_back(roistudy);
    }
  }

  if (roistudies.size() == 0) {
    std::cout << "No roi for those studies selected, nothing todo" << std::endl;
  }

}
// --------------------------------------------------------------------


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
  study.ReferenceCT = mhd;
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
      var[0] = var[1] = var[2] = GetGaussianVariance();
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
  std::string f(mDataPath+study.ReferenceCT);
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
void syd::sydQuery::ComputeRoiCumulActivity(Study study, int n)
{
  std::vector<RoiStudy> roistudies;
  LoadVector<RoiStudy>(roistudies, odb::query<RoiStudy>::StudyId == study.Id);
  for(auto i=roistudies.begin(); i<roistudies.end(); i++)
    ComputeRoiCumulActivity(study, *i, n);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeRoiCumulActivity(Study study, RoiStudy roistudy, int n)
{
  // Get all series for this patient/study
  std::vector<Serie> series;
  LoadVector<Serie>(series, odb::query<Serie>::StudyId == study.Id);

  // Get all roiseries for this roistudy
  std::vector<unsigned long> seriesId;
  for(auto i=series.begin(); i<series.end(); i++) seriesId.push_back(i->Id);
  std::map<unsigned long, Serie> seriesMap;
  for(auto i=series.begin(); i<series.end(); i++) seriesMap[i->Id] = *i;
  std::vector<RoiSerie> roiseries;
  LoadVector<RoiSerie>(roiseries,
                       (odb::query<RoiSerie>::RoiStudyId == roistudy.Id) &&
                       (odb::query<RoiSerie>::SerieId.in_range(seriesId.begin(), seriesId.end())));

  // Sort serie by time, then sort roiseries like serie.id
  struct less_than_time {
    inline bool operator() (const Serie & s1, const Serie & s2) {
      return (s1.TimeFromInjectionInHours < s2.TimeFromInjectionInHours);
    }
  };
  std::sort(seriesId.begin(), seriesId.end());
  std::sort(series.begin(), series.end(), less_than_time());
  struct less_than_serieId {
    std::map<unsigned long, Serie> seriesMap;
    less_than_serieId(std::map<unsigned long, Serie> &m):seriesMap(m) {}
    inline bool operator() (const RoiSerie & s1, const RoiSerie & s2) {
      return (seriesMap[s1.SerieId].TimeFromInjectionInHours < seriesMap[s2.SerieId].TimeFromInjectionInHours);
    }
  };
  std::sort(roiseries.begin(), roiseries.end(), less_than_serieId(seriesMap));

  // Get time + MeanActivity
  std::vector<double> times;
  for(auto i=series.begin(); i<series.end(); i++) times.push_back(i->TimeFromInjectionInHours);


  // one for total, one for max
  for(auto a=0; a<2; a++) {
    std::vector<double> activities;
    if (a==0) for(auto i=roiseries.begin(); i<roiseries.end(); i++) activities.push_back(i->TotalActivity);
    if (a==1) for(auto i=roiseries.begin(); i<roiseries.end(); i++) activities.push_back(i->MaxActivity);

    // Find time peak activity
    int peakindex=0;
    double peak = 0;
    for(auto i=0; i<activities.size(); i++) {
      if (activities[i] > peak) {
        peakindex = i;
        peak = activities[i];
      }
    }

    int m = activities.size() - peakindex;
    m = std::min(m, n);

    // Check if at least 2 points
    assert(m>1);

    // Compute integral
    double rms;
    double lambda;
    double A;
    if (a==0) {
      roistudy.CumulatedActivity = Tac_Integrate(times, activities, m, rms, A, lambda);
      roistudy.FitLambda = lambda;
      roistudy.FitA = A;
      roistudy.FitRMS = rms;
      roistudy.FitPeakTime = times[peakindex];
    }
    //    if (a==1) roistudy.CumulatedMaxActivity = Tac_Integrate(times, activities, m, rms, A, lambda);
  }


  // Update
  odb::transaction t (db->begin());
  db->update(roistudy);
  t.commit();

  // Verbose
  if (GetVerboseFlag()) {
    Patient p;
    Load<Patient>(p, odb::query<Patient>::Id == study.PatientId); // FIXME replace by GetFromId
    RoiType r;
    Load<RoiType>(r, odb::query<RoiType>::Id == roistudy.RoiTypeId);
    std::cout << p.SynfrizzId << " " << study.Number << " "
              << r.Name << " "
              << roistudy.CumulatedActivity << " "
              << roistudy.FitLambda << " "
              << roistudy.FitA << " "
              << roistudy.FitRMS << " "
              << roistudy.FitPeakTime << std::endl;
  }

  // CHECK
  if (0) {
    // parallelogram part is not very precise.
    // times.clear();
    // times.resize(8);
    // times = { 1, 2, 4, 6, 10, 11, 62, 130};
    std::vector<double> ac;
    double A = 1000;
    for(auto i=0; i<times.size(); i++) {
      ac.push_back(Tac_MonoExpo(A, Lambda_Indium, times[i]));
    }
    DDS(ac);
    DD(A);
    DD(Lambda_Indium);
    double tot = Tac_Integrate(A, Lambda_Indium);
    DD(tot);
    int n=2;
    double rms;
    double lambda;
    double tot2 = Tac_Integrate(times, ac, n, rms, A, lambda);

    //for(auto i=0; i<times.size()-1; i++)
    //  std::cout << i << " " << Tac_Integrate(A, lambda, times[i], times[i+1]) << std::endl;

    DD(tot2);
    DD(A);
    DD(lambda);
    DD(rms);

    // start part
    double st = Tac_Integrate(A, lambda, 0, times[0]);
    DD(st);
    double middle = Tac_Integrate(A, lambda, times[0], times.back());
    DD(middle);
    DD(tot-middle-st);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeCumulActivityImage(Study study)
{
  // Get 6 mhd + vector of times

  // Get all series for this patient/study
  std::vector<Serie> series;
  LoadVector<Serie>(series, odb::query<Serie>::StudyId == study.Id);

  // Get time
  std::vector<double> times;
  for(auto i=series.begin(); i<series.end(); i++) times.push_back(i->TimeFromInjectionInHours);

  // open the images
  std::vector<ImageType::Pointer> spect;
  for(auto i=series.begin(); i<series.end(); i++) {
    std::string f = std::string (mDataPath+i->MHDFilename);
    spect.push_back(clitk::readImage<ImageType>(f));
  }

  // create output image
  auto output = clitk::NewImageLike<ImageType>(spect[0], true);

  // Loop with 7 iterators ? to build the vector of activities ?
  // No a single iterator use GetBufferPointer
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
    m = std::min(m, 3); // 3 is fixed here
    double v = Tac_Integrate(times, activities, m, rms, A, lambda);
    if (isnan(v)) v = 0;
    if (activities[peakindex] < 500) v = 0; // dont look at lower activity
    if (v<0) v = 0; // no negative
    if (v>1000000) v = 1000000; // no too high
    iter.Set(v);
    ++t;
    ++iter;
  }

  // Compute output filename
  std::string f = std::string(mDataPath+series[0].MHDFilename);
  syd::replace(f, "spect1", "cumul");
  if (GetVerboseFlag()) { std::cout << f << std::endl; }
  clitk::writeImage<ImageType>(output, f);
  // Also save A lambda rms image ?
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
void syd::sydQuery::ComputeRoiCumulActivity2(unsigned long roistudyId, int n)
{
  // Get all objects associated with this roistudy
  RoiStudy roistudy = GetById<RoiStudy>(roistudyId);
  Study study = GetById<Study>(roistudy.StudyId);
  Patient patient = GetById<Patient>(study.PatientId);
  RoiType roitype = GetById<RoiType>(roistudy.RoiTypeId);

  // Get all roiseries
  std::vector<RoiSerie> roiseries;
  LoadVector<RoiSerie>(roiseries, odb::query<RoiSerie>::RoiStudyId == roistudy.Id);

  // Sort serie by time by time from injection
  struct was_injected_before {
    was_injected_before(sydQuery * s):q(s) {}
    sydQuery * q;
    inline bool operator() (const RoiSerie & s1, const RoiSerie & s2) {
      double s1Time = (q->GetById<Serie>(s1.SerieId)).TimeFromInjectionInHours;
      double s2Time = (q->GetById<Serie>(s2.SerieId)).TimeFromInjectionInHours;
      return (s1Time < s2Time);
    }
  };
  std::sort(roiseries.begin(), roiseries.end(), was_injected_before(this));

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
void syd::sydQuery::DumpRoiInformation(unsigned long SynfrizzId,
                                       std::vector<std::string> roinames)
{
  DD("DumpRoiInformation");
  DD(roinames.size());
  FATAL("TODO");

  // Get Patient from synfrizz id
  Patient patient;
  Load<Patient>(patient, odb::query<Patient>::SynfrizzId == SynfrizzId);

  // Get study from patient
  Study study;
  Load<Study>(study, odb::query<Study>::PatientId == patient.Id);

  // Build query
  for(auto i=roinames.begin(); i!= roinames.end(); i++) {
    // all roitype
    // all roistudies
  }

  RoiType roitype;
  // FIXME  Load<RoiType>(roitype, odb::query<RoiType>::Name == *i);

  // Get RoiStudy from roitype and study
  std::vector<RoiStudy> roistudies;
  //  LoadVector<RoiStudy>(roistudies, query);



  //    RoiStudy s = syd.GetRoiStudy(SynfrizzId, *i)
  //}

  //  DumpRoiInformation(*i);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::DumpRoiInformation(RoiStudy roistudy)
{
  DD("DumpRoiInformation");

  FATAL("TODO");
  // select RoiStudy

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::DumpStudy(Study study)
{
  // Get Patient info
  Patient p;
  Load<Patient>(p, odb::query<Patient>::Id == study.PatientId);

  // Get WholeImage info
  RoiStudy roistudy = GetRoiStudy(study, "WholeImage");

  // Get all series
  std::vector<Serie> series;
  LoadVector<Serie>(series, odb::query<Serie>::StudyId == study.Id);

  //  First line : SynfrizzId initial StudyNb InjectedQuantityInMBq count_wholebody
  std::cout << p.Id << " "
            << p.Name << " "
            << study.Number << " "
            << study.InjectedQuantityInMBq << " MBq " << std::endl;

  // 1 line by roi : name volume mass (later = cumulated activity)
  std::vector<RoiStudy> roistudies;
  LoadVector<RoiStudy>(roistudies, odb::query<RoiStudy>::StudyId == study.Id);
  std::cout.setf(std::ios::fixed, std::ios::floatfield); // set fixed floating format
  for(auto i = roistudies.begin(); i<roistudies.end(); i++) {
    RoiType roitype;
    Load<RoiType>(roitype, odb::query<RoiType>::Id == i->RoiTypeId);
    std::cout << std::setw(10) << roitype.Name << " "
              << std::setw(6) << std::setprecision(0) << i->VolumeInCC << " cc "
              << std::setw(7) << std::setprecision(1) << i->MassInGram << " g";// << std::endl;

    // Select series for this RoiStudy
    for(auto j = series.begin(); j<series.end(); j++) {
      RoiSerie roiserie;
      Load<RoiSerie>(roiserie,
                     odb::query<RoiSerie>::SerieId == j->Id &&
                     odb::query<RoiSerie>::RoiStudyId == i->Id);
      std::cout << std::setw(14) << std::setprecision(0) << roiserie.TotalActivity;
    }
    std::cout << std::endl;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::DumpCalibrationFactor()
{
  // Get patients
  std::vector<Patient> patients;
  LoadVector<Patient>(patients, odb::query<Patient>::SynfrizzId != 9999); // all

  // Loop patient
  for(auto i=patients.begin(); i<patients.end(); i++) {
    // Search for studies
    std::vector<Study> studies;
    LoadVector<Study>(studies, odb::query<Study>::PatientId == i->Id);

    // Loop for studies
    for(auto j=studies.begin(); j<studies.end(); j++) {
      DumpCalibrationFactor(*j);
    }
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::DumpCalibrationFactor(Study study)
{
  // Get Patient
  Patient p;
  Load<Patient>(p, odb::query<Patient>::Id == study.PatientId);

  // Get wholeimage
  RoiStudy roistudy;
  //if (p.SynfrizzId == 0)
  roistudy = GetRoiStudy(study, "WholeImage");
  //  else roistudy = GetRoiStudy(study, "Patient");

  // Get all series
  std::vector<Serie> series;
  LoadVector<Serie>(series, odb::query<Serie>::StudyId == study.Id);

  // Find first serie
  Serie serie;
  double oldest = 9999999999999;
  for(auto i=series.begin(); i<series.end(); i++) {
    if (i->TimeFromInjectionInHours < oldest) {
      oldest = i->TimeFromInjectionInHours;
      serie = *i;
    }
  }

  // Get roiserie
  RoiSerie roiserie;
  Load<RoiSerie>(roiserie, odb::query<RoiSerie>::SerieId == serie.Id &&
                 odb::query<RoiSerie>::RoiStudyId == roistudy.Id);

  // Compute theoretical MBq at time
  double activity = Tac_MonoExpo(study.InjectedQuantityInMBq, Lambda_Indium,
                                 serie.TimeFromInjectionInHours);
  double k = roiserie.TotalActivity/activity;

  //  Dump info
  std::cout.setf(std::ios::fixed, std::ios::floatfield); // set fixed floating format
  std::cout << std::setw(3) << p.SynfrizzId
            << std::setw(3) << study.Number
            << std::setw(8) << std::setprecision(1) << study.InjectedQuantityInMBq << " MBq"
            << std::setw(8) << std::setprecision(1) << serie.TimeFromInjectionInHours << " h"
            << std::setw(7) << std::setprecision(1) << activity << " MBq"
            << std::setw(12) << std::setprecision(0) << roiserie.TotalActivity << " cts"
            << std::setw(12) << std::setprecision(0) << k << std::endl;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::DumpStudy2(Study study)
{

  // Get Patient info
  Patient p;
  Load<Patient>(p, odb::query<Patient>::Id == study.PatientId);

  // Load all series for this study
  std::vector<Serie> series;
  LoadVector<Serie>(series, odb::query<Serie>::StudyId == study.Id);

  // Load all roistudy for this study
  std::vector<RoiStudy> roistudies;
  LoadVector<RoiStudy>(roistudies, odb::query<RoiStudy>::StudyId == study.Id);

  // Struct to fill
  struct column {
    double time;
    std::vector<RoiSerie> roiseries;
  };
  std::map<double, column> lines;
  std::vector<RoiType> roitypes;

  // Loop over roi
  for(auto i = roistudies.begin(); i<roistudies.end(); i++) {
    RoiType roitype;
    Load<RoiType>(roitype, odb::query<RoiType>::Id == i->RoiTypeId);
    roitypes.push_back(roitype);

    // select serie for this roistudy
    for(auto j = series.begin(); j<series.end(); j++) {
      RoiSerie roiserie;
      Load<RoiSerie>(roiserie,
                     odb::query<RoiSerie>::SerieId == j->Id && // Or single query with ||
                     odb::query<RoiSerie>::RoiStudyId == i->Id);
      double t = j->TimeFromInjectionInHours;
      lines[t].roiseries.push_back(roiserie);
      lines[t].time = t; // FIXME Not needed
    }
  }

  // Dump header
  std::cout.setf(std::ios::fixed, std::ios::floatfield); // set fixed floating format
  std::cout << std::setw(6) << "Time";
  for(auto i=roitypes.begin(); i<roitypes.end(); i++) {
    std::cout << std::setw(22) << i->Name;
  }
  std::cout << std::endl;

  // ID%/g
  Property prop;
  Load<Property>(prop, odb::query<Property>::Name == "CalibrationFactor");
  double k = atof(prop.Value.c_str());
  k = 1.0/k;
  double ia = study.InjectedQuantityInMBq;

  // Dump
  for(auto i=lines.begin(); i != lines.end(); i++) { // for all times
    std::cout << std::setprecision(1) << std::setw(6) << i->first;//second.time;
    int l=0;
    // For all rois
    for (auto j=i->second.roiseries.begin(); j<i->second.roiseries.end(); j++) {
      double idpg = (j->MeanActivity*k/roistudies[l].Density)/(ia)*100;
      std::cout << std::setprecision(8) << std::setw(14) << idpg;
      l++;
    }
    std::cout << std::endl;
  }


}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::DumpActivity(std::string arg, std::vector<int> & ids, std::string type)
{
  // all roitype lesion or organs
  std::vector<RoiType> roitypes;
  if (arg == "large-lesions") {
    LoadVector<RoiType>(roitypes, odb::query<RoiType>::Name.like("LargeLesion%"));
  }
  else {
    if (arg == "lesions") {
      LoadVector<RoiType>(roitypes, odb::query<RoiType>::Name.like("Lesion%"));
    }
    else {
      RoiType r;
      Load<RoiType>(r, odb::query<RoiType>::Name == "LiverSphere"); roitypes.push_back(r);
      Load<RoiType>(r, odb::query<RoiType>::Name == "HeartSphere"); roitypes.push_back(r);
      Load<RoiType>(r, odb::query<RoiType>::Name == "SpleenSphere"); roitypes.push_back(r);
      Load<RoiType>(r, odb::query<RoiType>::Name == "RKidneySphere"); roitypes.push_back(r);
      Load<RoiType>(r, odb::query<RoiType>::Name == "LKidneySphere"); roitypes.push_back(r);
      Load<RoiType>(r, odb::query<RoiType>::Name == "BoneMarrow"); roitypes.push_back(r);
      Load<RoiType>(r, odb::query<RoiType>::Name == "Patient"); roitypes.push_back(r);
    }
  }
  std::vector<unsigned long> roitypeid;
  for(auto i=roitypes.begin(); i<roitypes.end(); i++) roitypeid.push_back(i->Id);

  // dump header
  int col1 = 4;
  int col = 14;
  std::cout << std::setw(col1) << "Pat";
  std::cout.setf(std::ios::fixed, std::ios::floatfield); // set fixed floating format
  for(auto i=roitypes.begin(); i<roitypes.end(); i++)
    std::cout << std::setw(col) << i->Name;
  std::cout << std::endl;

  // Get Patient from study
  std::vector<Patient> patients;
  LoadVector<Patient>(patients, odb::query<Patient>::Id.in_range(ids.begin(), ids.end()));

  Property prop;
  Load<Property>(prop, odb::query<Property>::Name == "CalibrationFactor");
  double k = atof(prop.Value.c_str());
  k = 1.0/k;

  // Loo pover patients
  for(auto i=patients.begin(); i<patients.end(); i++) {
    std::cout << std::setw(col1) << i->SynfrizzId;

    // Get study
    Study study = GetStudy(i->SynfrizzId, 0);

    double ia = study.InjectedQuantityInMBq;
    double totalAc = Tac_Integrate(ia, Lambda_Indium);

    // Loop on roi
    for(auto j=roitypes.begin(); j<roitypes.end(); j++) {
      std::vector<RoiStudy> roistudies;
      LoadVector<RoiStudy>(roistudies,
                           (odb::query<RoiStudy>::StudyId == study.Id) &&
                           (odb::query<RoiStudy>::RoiTypeId == j->Id));
      if (roistudies.size() == 1) {
        RoiStudy r = roistudies[0];
        double v;
        if (type == "total") v = (k*r.CumulatedActivity)/(ia*r.MassInGram);
        //        if (type == "max")   v = (k*r.CumulatedMaxActivity)/(ia*r.Density);
        std::cout << std::setw(col) << std::setprecision(1) << v*1000; // kBq.h/g / injectedMBq
      }
      else {
        std::cout << std::setw(col) << "-";
      }
    }

    std::cout << std::endl;
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::DumpEffectiveHalfLife(std::string arg, std::vector<int> & ids)
{

  // FIXME too similar to DumpActivity to be merged.

  // all roitype lesion or organs
  std::vector<RoiType> roitypes;
  if (arg == "lesions") {
    LoadVector<RoiType>(roitypes, odb::query<RoiType>::Name.like("%Lesion%"));
  }
  else {
    RoiType r;
    Load<RoiType>(r, odb::query<RoiType>::Name == "LiverSphere"); roitypes.push_back(r);
    Load<RoiType>(r, odb::query<RoiType>::Name == "HeartSphere"); roitypes.push_back(r);
    Load<RoiType>(r, odb::query<RoiType>::Name == "SpleenSphere"); roitypes.push_back(r);
    Load<RoiType>(r, odb::query<RoiType>::Name == "RKidneySphere"); roitypes.push_back(r);
    Load<RoiType>(r, odb::query<RoiType>::Name == "LKidneySphere"); roitypes.push_back(r);
    Load<RoiType>(r, odb::query<RoiType>::Name == "BoneMarrow"); roitypes.push_back(r);
    Load<RoiType>(r, odb::query<RoiType>::Name == "Patient"); roitypes.push_back(r);
  }
  std::vector<unsigned long> roitypeid;
  for(auto i=roitypes.begin(); i<roitypes.end(); i++) roitypeid.push_back(i->Id);

  // dump header
  int col1 = 4;
  int col = 14;
  std::cout << std::setw(col1) << "Pat";
  std::cout.setf(std::ios::fixed, std::ios::floatfield); // set fixed floating format
  for(auto i=roitypes.begin(); i<roitypes.end(); i++)
    std::cout << std::setw(col) << i->Name;
  std::cout << std::endl;

  // Get Patient from study
  std::vector<Patient> patients;
  LoadVector<Patient>(patients, odb::query<Patient>::Id.in_range(ids.begin(), ids.end()));

  // Loop over patients
  for(auto i=patients.begin(); i<patients.end(); i++) {
    std::cout << std::setw(col1) << i->SynfrizzId;

    // Get study
    Study study = GetStudy(i->SynfrizzId, 0);

    // Loop on roi
    for(auto j=roitypes.begin(); j<roitypes.end(); j++) {
      std::vector<RoiStudy> roistudies;
      LoadVector<RoiStudy>(roistudies,
                           (odb::query<RoiStudy>::StudyId == study.Id) &&
                           (odb::query<RoiStudy>::RoiTypeId == j->Id));
      if (roistudies.size() == 1) {
        RoiStudy r = roistudies[0];
        double v = log(2)/r.FitLambda;
        std::cout << std::setw(col) << std::setprecision(3) << v; // hour
      }
      else {
        std::cout << std::setw(col) << "-";
      }
    }

    std::cout << std::endl;
  }

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::DumpPatientRoisValue(std::string arg, std::vector<int> & ids, std::string type)
{
  // all roitype lesion or organs
  std::vector<RoiType> roitypes;
  if ((type == "LesionLiverRatio") || (type == "LesionLiverPeakRatio")) {
    RoiType r;
    Load<RoiType>(r, odb::query<RoiType>::Name == "LiverSphere"); roitypes.push_back(r);
  }

  if (arg == "large-lesions") {
    LoadVector<RoiType>(roitypes, odb::query<RoiType>::Name.like("LargeLesion%"));
  }
  else {
    if (arg == "alllesions") {
      LoadVector<RoiType>(roitypes, odb::query<RoiType>::Name.like("%Lesion%"));
    }
    else {
      if (arg == "lesions") {
        LoadVector<RoiType>(roitypes, odb::query<RoiType>::Name.like("Lesion%"));
      }
      else {
        RoiType r;
        Load<RoiType>(r, odb::query<RoiType>::Name == "LiverSphere"); roitypes.push_back(r);
        Load<RoiType>(r, odb::query<RoiType>::Name == "HeartSphere"); roitypes.push_back(r);
        Load<RoiType>(r, odb::query<RoiType>::Name == "SpleenSphere"); roitypes.push_back(r);
        Load<RoiType>(r, odb::query<RoiType>::Name == "RKidneySphere"); roitypes.push_back(r);
        Load<RoiType>(r, odb::query<RoiType>::Name == "LKidneySphere"); roitypes.push_back(r);
        Load<RoiType>(r, odb::query<RoiType>::Name == "BoneMarrow"); roitypes.push_back(r);
        Load<RoiType>(r, odb::query<RoiType>::Name == "Patient"); roitypes.push_back(r);
      }
    }
  }

  std::vector<unsigned long> roitypeid;
  for(auto i=roitypes.begin(); i<roitypes.end(); i++) roitypeid.push_back(i->Id);

  // dump header
  int col1 = 4;
  int col = 14;
  std::cout << std::setw(col1) << "Pat";
  std::cout.setf(std::ios::fixed, std::ios::floatfield); // set fixed floating format
  for(auto i=roitypes.begin(); i<roitypes.end(); i++) {
    if (i->Name == "LiverSphere") {
      if ((type == "LesionLiverRatio") || (type == "LesionLiverPeakRatio")) {}
      else std::cout << std::setw(col) << i->Name;
    }
    else std::cout << std::setw(col) << i->Name;
  }
  std::cout << std::endl;

  // Get Patient from study
  std::vector<Patient> patients;
  LoadVector<Patient>(patients, odb::query<Patient>::Id.in_range(ids.begin(), ids.end()));

  Property prop;
  Load<Property>(prop, odb::query<Property>::Name == "CalibrationFactor");
  double k = atof(prop.Value.c_str());
  k = 1.0/k;

  // Loo pover patients
  double livermeanactivity;
  for(auto i=patients.begin(); i<patients.end(); i++) {
    std::cout << std::setw(col1) << i->SynfrizzId;

    // Get study
    Study study = GetStudy(i->SynfrizzId, 0);

    double ia = study.InjectedQuantityInMBq;
    double totalAc = Tac_Integrate(ia, Lambda_Indium);

    // Loop on roi
    for(auto j=roitypes.begin(); j<roitypes.end(); j++) {
      std::vector<RoiStudy> roistudies;
      LoadVector<RoiStudy>(roistudies,
                           (odb::query<RoiStudy>::StudyId == study.Id) &&
                           (odb::query<RoiStudy>::RoiTypeId == j->Id));
      if (roistudies.size() == 1) {
        RoiStudy r = roistudies[0];
        double v;

        // specific case for LesionLiverRatio
        if (j->Name == "LiverSphere") {
          if ((type == "LesionLiverRatio") || (type == "LesionLiverPeakRatio")) {
            livermeanactivity = (k*r.CumulatedActivity)/(ia*r.MassInGram)*1000;
            // Do not write anything
          }
        }
        else {
          double meanactivity = (k*r.CumulatedActivity)/(ia*r.MassInGram)*1000; // kBq.h/g / injectedMBq
          //          double peakactivity = (k*r.CumulatedMaxActivity)/(ia*r.Density)*1000;// kBq.h/g / injectedMBq
          if (type == "total") v = meanactivity;
          //          if (type == "max")   v = peakactivity;
          if (type == "effHL") v =  log(2)/r.FitLambda;

          if (type == "LesionLiverRatio") {
            v = meanactivity/livermeanactivity;
            std::cout << std::setw(col) << std::setprecision(3) << v;
          }
          else {
            if (type == "LesionLiverPeakRatio") {
              //              v = peakactivity/livermeanactivity;
              std::cout << std::setw(col) << std::setprecision(3) << v;
            }
            else std::cout << std::setw(col) << std::setprecision(1) << v;
          }
        }
      }
      else {
        std::cout << std::setw(col) << "-";
      }
    }

    std::cout << std::endl;
  }

}
// --------------------------------------------------------------------
