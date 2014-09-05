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
        //        printf ("Token: %s\n", s);
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
void syd::sydQuery::Get_Resampled_Mask(RoiStudy roistudy, ImageType::Pointer spect,
                                       MaskImageType::Pointer & initialmask,
                                       MaskImageType::Pointer & resampledmask)
{
  RoiType roitype = GetById<RoiType>(roistudy.RoiTypeId);
  bool nomask = false;
  if (roitype.Name == "WholeImage") nomask = true;

  // read mask
  std::string f = std::string (mDataPath+roistudy.MHDFilename);
  if (!nomask) initialmask = clitk::readImage<MaskImageType>(f);
  else { // create a "full" mask
    initialmask = MaskImageType::New();
    initialmask->CopyInformation(spect);
    initialmask->SetRegions(spect->GetLargestPossibleRegion());
    initialmask->Allocate();
    initialmask->FillBuffer(1);
  }

  // Resample mask like spect (with NN interpolation)
  resampledmask = clitk::ResampleImageLike<MaskImageType>(initialmask, spect, 0, 0); // O is BG, 0 is NN interpolation
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::Get_RoiSeries_Sorted_by_Time(RoiStudy roistudy, std::vector<RoiSerie> & roiseries)
{
  // First load the series
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
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydQuery::GetSortedSeries(Study study, std::vector<Serie> & series)
{
  // First load the series
  LoadVector<Serie>(series, odb::query<Serie>::StudyId == study.Id);

  // Sort serie by time by time from injection
  struct was_injected_before {
    was_injected_before(sydQuery * s):q(s) {}
    sydQuery * q;
    inline bool operator() (const Serie & s1, const Serie & s2) {
      double s1Time = s1.TimeFromInjectionInHours;
      double s2Time = s2.TimeFromInjectionInHours;
      return (s1Time < s2Time);
    }
  };
  std::sort(series.begin(), series.end(), was_injected_before(this));
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
        //if (type == "total") v = (k*r.CumulatedActivity)/(ia*r.MassInGram);
        if (type == "total") v = (k*r.TimeIntegratedMeanActivity)/(ia*r.Density);
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
            livermeanactivity = (k*r.TimeIntegratedMeanActivity)/(ia*r.MassInGram)*1000;
            // Do not write anything
          }
        }
        else {
          double meanactivity = (k*r.TimeIntegratedMeanActivity)/(ia*r.MassInGram)*1000; // kBq.h/g / injectedMBq
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
