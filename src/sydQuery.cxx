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
void syd::sydQuery::GetListOfRois(std::string roiname, std::vector<std::string> & rois)
{
  // if (roiname == "all") {
  //   typedef odb::query<RoiType> q;
  //   std::vector<RoiType> p;
  //   LoadVector<RoiType>(p, q::Name != "");
  //   for(auto i=p.begin(); i<p.end(); i++)
  //     rois.push_back(i->Name);
  // }
  // else rois.push_back(roiname);
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
    std::cout << "Add RoiStudy " << r.Id << " " << r.RoiTypeId << " " << r.StudyId << " " << mhd ;
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
    if (update) std::cout << "Update ";
    else std::cout << "Insert ";
    std::cout << "serie " << serie.Id << " with " << dcm << std::endl;
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
    std::cout << "Add CT " << mhd << " to study " << study.Id << std::endl;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::ComputeRoiTimeActivity(Study study)
{
  std::vector<RoiStudy> roistudies;
  LoadVector<RoiStudy>(roistudies, odb::query<RoiStudy>::StudyId == study.Id);
  for(auto i=roistudies.begin(); i<roistudies.end(); i++)
    ComputeRoiTimeActivity(*i);
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
    res.push_back(statisticsFilter->GetCount(1));
    double m = statisticsFilter->GetMean(1);
    // mean by pixel to convert into mean by cc (spacing is in mm, so x0.001)
    double vol = (spect->GetSpacing()[0]*spect->GetSpacing()[1]*spect->GetSpacing()[2]*0.001);
    m = m/vol;
    res.push_back(m);
    res.push_back(statisticsFilter->GetSigma(1)/vol);
    res.push_back(statisticsFilter->GetSum(1));

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
    means.push_back(r.MeanActivity);
    tot.push_back(r.TotalActivity);
    db->update(r);
    ++n;
  }
  t.commit();

  // Verbose
  if (GetVerboseFlag()) {
    std::cout << "Mean counts : ";
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
void syd::sydQuery::ComputeRoiCumulActivity(RoiStudy roistudy)
{
  DD("ComputeRoiActivity");

  FATAL("TODO");

  /*
  // No need to check fov -> select first
  std::vector<Serie> series; // list of 5 or 6 serie
  {
    typedef odb::query<Serie> q;
    LoadVector<Serie>(series, q::StudyId == roistudy.StudyId && q::FOV == "fov1");
    if (series.size() == 0) FATAL("Could not find series ? " << std::endl << mCurrentSQLQuery);
  }
  */
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
  double activity = MonoExpo(study.InjectedQuantityInMBq, serie.TimeFromInjectionInHours, Lambda_Indium);
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
