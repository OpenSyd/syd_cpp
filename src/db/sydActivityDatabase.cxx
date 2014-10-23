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
#include "sydActivityDatabase.h"

// --------------------------------------------------------------------
syd::ActivityDatabase::ActivityDatabase(std::string name, std::string param):Database(name)
{
  // List all params
  std::istringstream f(param);
  SetFileAndFolder(f);
  std::string cdb_name;
  if (!getline(f, cdb_name, ';')) {
    LOG(FATAL) << "Error while parsing cdb name for folder. db is "
               << name << " (" << get_typename() << ") params = " << param;
  }
  std::string sdb_name;
  if (!getline(f, sdb_name, ';')) {
    LOG(FATAL) << "Error while parsing sdb name for folder. db is "
               << name << " (" << get_typename() << ") params = " << param;
  }

  OpenSqliteDatabase(filename_, folder_);

  cdb_ = syd::Database::OpenDatabaseType<ClinicDatabase>(cdb_name);
  sdb_ = syd::Database::OpenDatabaseType<StudyDatabase>(sdb_name);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ActivityDatabase::~ActivityDatabase()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::CreateDatabase()
{
  DD("CreateDatabase");

  // DD("here");
  // odb::transaction t (db_->begin());
  // odb::schema_catalog::create_schema (*db_);
  // t.commit();

  DD("TODO");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::CheckIntegrity(std::vector<std::string> & args)
{
  DD("TODO ActivityDatabase::CheckIntegrity");
  DDS(args);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Activity syd::ActivityDatabase::NewActivity(const Patient & patient)
{
  Activity activity;
  activity.patient_id = patient.id;
  RawImage ct;
  ct.patient_id = patient.id;
  ct.md5 = "";
  ct.pixel_type = "short";
  ct.filename = "average.mhd";
  DD("TODO");
  exit(0);
  //FIXME
  ct.path = patient.name+PATH_SEPARATOR;
  Insert(ct);
  // activity.average_ct_image_id = ct.id;
  Insert(activity);
  return activity;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TimeActivity syd::ActivityDatabase::NewTimeActivity(const Timepoint & timepoint,
                                                         const RoiMaskImage & roi)
{
  Patient patient(sdb_->GetPatient(timepoint));
  TimeActivity ta;
  ta.timepoint_id = timepoint.id;
  ta.patient_id = patient.id;
  ta.roi_id = roi.id;
  Insert(ta);
  return ta;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::Dump(std::ostream & os, std::vector<std::string> & args)
{
  if (args.size() < 2) {
    LOG(FATAL) << "Error need <patient> and <roiname>";
  }

  // patient
  std::vector<Patient> patients;
  std::string patientname = args[0];
  cdb_->GetPatientsByName(patientname, patients);

  // Loop on patient
  for(auto patient:patients) Dump(os, patient, args);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::Dump(std::ostream & os, const Patient & patient, std::vector<std::string> & args)
{
  // Get first timepoint
  Timepoint timepoint;
  bool b = sdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::number == 1 and
                                       odb::query<Timepoint>::patient_id == patient.id, timepoint);
  if (!b) {
    return; //LOG(FATAL) << "Error no timepoint with number 1 for patient " << patient.name;
  }

  // Get roi
  std::string roiname = args[1];
  std::vector<RoiMaskImage> roimaskimages =
    sdb_->GetRoiMaskImages(timepoint, roiname);
  if (roimaskimages.size() == 0) {
    return; //LOG(FATAL) << "No roimaskimage found with name '" << roiname << "'";
  }
  // Sort by id
  std::sort(begin(roimaskimages), end(roimaskimages),
            [&roimaskimages](RoiMaskImage a, RoiMaskImage b) { return a.roitype_id < b.roitype_id; }  );


  // Prepare to print
  syd::PrintTable ta;
  ta.AddColumn("Nb", 3, 0);
  ta.AddColumn("t", 9, 1);

  // loop on roi to get all timeactivities
  int nb = 0;
  std::vector<std::vector<TimeActivity>> tva;
  for(auto roi:roimaskimages) {
    // Retrieve all timepoints and sort
    std::vector<TimeActivity> timeactivities;
    LoadVector<TimeActivity>(odb::query<TimeActivity>::patient_id == patient.id and
                             odb::query<TimeActivity>::roi_id == roi.id, timeactivities);
    std::sort(begin(timeactivities), end(timeactivities),
              [this](TimeActivity a, TimeActivity b) {
                Timepoint ta(sdb_->GetById<Timepoint>(a.timepoint_id));
                Timepoint tb(sdb_->GetById<Timepoint>(b.timepoint_id));
                return ta.number < tb.number; }  );
    if (timeactivities.size() > nb) nb = timeactivities.size();
    tva.push_back(timeactivities);
    RoiType roitype(sdb_->GetRoiType(roi));
    ta.AddColumn(roitype.name, 11, 1);
    ta.AddColumn("std/cc", 7, 2);
  }

  // get mean activity
  ta.Init();
  for(auto i=0; i<nb; i++) {
    Timepoint t(sdb_->GetById<Timepoint>(tva[0][i].timepoint_id));
    ta << t.number
       << t.time_from_injection_in_hours;
    for(auto j=0; j<roimaskimages.size(); j++) {
      RoiMaskImage roi(roimaskimages[j]);
      if (i<tva[j].size()) {
        TimeActivity activity(tva[j][i]);
        ta << activity.mean_counts_by_cc
           << activity.std_counts_by_cc;
      }
      else {
        ta << "-" << "-";
      }
    }
  }

  // Final indication of (first) roi size
  RoiMaskImage roi(roimaskimages[0]);
  RoiType roitype(sdb_->GetRoiType(roi));
  std::cout << "# " << patient.name << " " << patient.synfrizz_id << " "
            << roitype.name << " " << roi.volume_in_cc << " cc "
            << roi.density_in_g_cc*roi.volume_in_cc << " g" << std::endl;
  ta.Print(std::cout);
};
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ActivityDatabase::UpdateTimeActivityInRoi(TimeActivity & timeactivity)
{
  // Get corresponding timepoint and roi
  Timepoint timepoint(sdb_->GetById<Timepoint>(timeactivity.timepoint_id));
  RoiMaskImage roi(sdb_->GetById<RoiMaskImage>(timeactivity.roi_id));

  // Load spect image
  RawImage ispect = sdb_->GetById<RawImage>(timepoint.spect_image_id);
  std::string fspect = sdb_->GetImagePath(ispect);
  ImageType::Pointer spect = syd::ReadImage<ImageType>(fspect);

  // Load roi mask
  std::string fmask = sdb_->GetImagePath(roi);
  MaskImageType::Pointer mask = syd::ReadImage<MaskImageType>(fmask);

  // in general spect and mask not the same spacing. Need to resample
  // one of the two.

  // resample mask like the spect image (both spacing and crop).
  // FIXME or reverse ... --> but need to store correct volume for
  // the counts if resample, need to convert values in
  // counts_concentration
  mask = syd::ResampleImageLike<MaskImageType>(mask, spect, 0, 0);

  // compute stats
  typedef itk::LabelStatisticsImageFilter<ImageType, MaskImageType> FilterType;
  typename FilterType::Pointer filter=FilterType::New();
  filter->SetInput(spect);
  filter->SetLabelInput(mask);
  filter->Update();

  // Should I update also roi values (vol + density) ? No.
  double pixelVol = spect->GetSpacing()[0]*spect->GetSpacing()[1]*spect->GetSpacing()[2];
  double vol = filter->GetCount(1) * pixelVol * 0.001; // in CC

  // Store stats
  timeactivity.mean_counts_by_cc = filter->GetMean(1)/pixelVol; // mean counts by cc
  timeactivity.std_counts_by_cc = filter->GetSigma(1)/pixelVol; // std deviation by cc

  // Also peak here ? no. Not here : prefer for integrated activity)

  // Update db
  Update(timeactivity);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
