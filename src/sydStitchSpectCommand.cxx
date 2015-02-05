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
#include "sydStitchSpectCommand.h"
#include "sydImage.h"

// --------------------------------------------------------------------
syd::StitchSpectCommand::StitchSpectCommand(std::string db):DatabaseCommand()
{
  sdb_ = syd::Database::OpenDatabaseType<StudyDatabase>(db);
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::StitchSpectCommand::StitchSpectCommand(syd::StudyDatabase  * db):
  DatabaseCommand(), sdb_(db)
{
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StitchSpectCommand::Initialization()
{
  cdb_ = sdb_->get_clinical_database();
  set_threshold_cumul(150000);
  set_skip_slices(4);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::StitchSpectCommand::~StitchSpectCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StitchSpectCommand::StitchSpect(std::vector<std::string> inputs)
{
  // Convert serie to u long
  std::vector<IdType> ids;
  for(auto i=0; i<inputs.size(); i++) {
    IdType id = toULong(inputs[i]);
    ids.push_back(id);
  }

  // Insert all series
  for(auto i: ids) {
    Serie serie = cdb_->GetById<Serie>(i);
    StitchSpect(serie);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StitchSpectCommand::StitchSpect(const Serie & serie)
{
  // Check modality
  if (serie.modality != "NM") {
    LOG(FATAL) << "Error the serie " << serie.id << " modality is " << serie.modality
               << " while expecting NM";
  }

  // Get patient
  Patient patient(cdb_->GetById<Patient>(serie.patient_id));

  // Compute time_from_injection_in_hours
  double time =
    syd::DateDifferenceInHours(serie.acquisition_date, patient.injection_date);

  // Get all the timepoints for this patient
  std::vector<Timepoint> timepoints;
  sdb_->LoadVector<Timepoint>(odb::query<Timepoint>::patient_id == patient.id, timepoints);
  if (timepoints.size() == 0) {
    LOG(FATAL) << "Sorry, could not find an existing timepoint for this patient " << patient.name;
  }
  Timepoint timepoint;
  double min_diff = 1000.0;
  for(auto t:timepoints) {
    double diff = fabs(time - t.time_from_injection_in_hours);
    if (diff < min_diff) {
      timepoint = t;
      min_diff = diff;
    }
  }
  ELOG(2) << "I found the timepoint " << timepoint.number << " for patient " << patient.name;

  // Consider the dicom spect filename
  std::string dicom_filename = cdb_->GetSeriePath(serie.id);

  // Create temporary mhd file
  RawImage spect(sdb_->GetById<RawImage>(timepoint.spect_image_id));
  std::string spect_mhd_filename = sdb_->GetImagePath(spect);
  std::string tmp_mhd_filename = spect_mhd_filename+"_tostitch.mhd";
  syd::ConvertDicomSPECTFileToImage(dicom_filename, tmp_mhd_filename);

  // Make a copy
  syd::CopyMHDImage(spect_mhd_filename, spect_mhd_filename+"_copy.mhd");

  // Stitch
  ImageType::Pointer s1 = syd::ReadImage<ImageType>(spect_mhd_filename);
  ImageType::Pointer s2 = syd::ReadImage<ImageType>(tmp_mhd_filename);
  ImageType::Pointer output = syd::StitchImages<ImageType>(s1,s2, get_threshold_cumul(), get_skip_slices());

  // Insert into the db
  ELOG(1) << "Updating spect image " << timepoint.number  << " for patient " << patient.name;
  syd::WriteImage<ImageType>(output, spect_mhd_filename);
  sdb_->UpdateMD5(spect);

}
// --------------------------------------------------------------------
