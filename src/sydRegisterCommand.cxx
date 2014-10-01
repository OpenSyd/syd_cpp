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
#include "sydRegisterCommand.h"

// --------------------------------------------------------------------
syd::RegisterCommand::RegisterCommand(StudyDatabase * d1, StudyDatabase * d2):
  DatabaseCommand(), in_db_(d1), out_db_(d2)
{
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RegisterCommand::RegisterCommand(std::string d1, std::string d2):
  DatabaseCommand()
{
  in_db_ = syd::Database::OpenDatabaseType<StudyDatabase>(d1);
  out_db_ = syd::Database::OpenDatabaseType<StudyDatabase>(d2);
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RegisterCommand::Initialization()
{
  cdb_ = in_db_->get_clinical_database();
  if (cdb_->get_name() != out_db_->get_clinical_database()->get_name()) {
    LOG(FATAL) << "Error the associated clinical database of " << in_db_->get_name()
               << " is different from the one in " << out_db_->get_name();
  }
  config_filename_ = "noconfigfile";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RegisterCommand::~RegisterCommand()
{

}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::RegisterCommand::Run(std::string patient_name, int ref_number, int mov_number)
{
  DD("run");

  // Get the patient
  Patient patient;
  if (!cdb_->GetIfExist<Patient>(odb::query<Patient>::name == patient_name, patient)) {
    LOG(FATAL) << "Error, the patient " << patient_name << " does not exist";
  }

  // Get the timepoints
  Timepoint reference_timepoint;
  bool b = in_db_->GetIfExist<Timepoint>(odb::query<Timepoint>::patient_id == patient.id &&
                                         odb::query<Timepoint>::number == ref_number,
                                         reference_timepoint);
  if (!b) {
    LOG(FATAL) << "Error could not find the (reference) timepoint number "
               << ref_number << " for the patient " << patient.name;
  }
  Timepoint moving_timepoint;
  b = in_db_->GetIfExist<Timepoint>(odb::query<Timepoint>::patient_id == patient.id &&
                                     odb::query<Timepoint>::number == mov_number,
                                     moving_timepoint);
  if (!b) {
    LOG(FATAL) << "Error could not find the (moving) timepoint number "
               << mov_number << " for the patient " << patient.name;
  }
  DD(reference_timepoint);
  DD(moving_timepoint);

  // Create the command line to register
  Run(reference_timepoint, moving_timepoint);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RegisterCommand::Run(Timepoint in_ref, Timepoint in_mov)
{
  // A single timepoint by patient
  if (in_ref.patient_id != in_mov.patient_id) {
    LOG(FATAL) << "Error the two timepoints are associated with 2 different patients : " << in_ref
               << " and " << in_mov;
  }

  // Copy or update reference tp in the new db
  Timepoint out_ref;
  bool b = out_db_->GetIfExist<Timepoint>(odb::query<Timepoint>::spect_serie_id == in_ref.spect_serie_id, out_ref);

  if (!b) { // Does not exist, create
    VLOG(1) << "Creating copy of " << in_db_->Print(in_ref);
    RawImage in_spect(in_db_->GetById<RawImage>(in_ref.spect_image_id));
    RawImage in_ct(in_db_->GetById<RawImage>(in_ref.ct_image_id));
    RawImage out_spect(in_spect);
    RawImage out_ct(in_ct);
    out_ref.copy(in_ref);
    out_db_->InsertTimepoint(out_ref, out_spect, out_ct);
  }
  else { // already exist, check md5
    VLOG(1) << "Already existing ref timepoint, updating : " << out_db_->Print(out_ref);
  }

  // Copy files (will check md5 before copy)
  in_db_->CopyFilesTo(in_ref, out_db_, out_ref);

  // Create new ot update moving tp in the new db
  Timepoint out_mov;
  b = out_db_->GetIfExist<Timepoint>(odb::query<Timepoint>::spect_serie_id == in_mov.spect_serie_id, out_mov);

  if (!b) { // Does not exist, create
    VLOG(1) << "Creating new moving tp of " << in_db_->Print(in_mov);
    RawImage in_spect(in_db_->GetById<RawImage>(in_mov.spect_image_id));
    RawImage in_ct(in_db_->GetById<RawImage>(in_mov.ct_image_id));
    RawImage out_spect(in_spect);
    RawImage out_ct(in_ct);
    out_spect.md5 = ""; // no image yet
    out_ct.md5 = "";    // no image yet
    out_mov.copy(in_mov);
    out_db_->InsertTimepoint(out_mov, out_spect, out_ct);
  }


  DD(out_mov);
  /*
    if new : look into folder -> compute md5

   */

  // else { // already exist, updating
  //   if (out_db_->FilesExist(out_mov)) { // check md5
  //     if (out_db_->CheckMD5(out_mov)) {
  //       VLOG(1) << "MD5 files ok, nothing to do.";
  //     }
  //     else {
  //       VLOG(1) << "MD5 files are different : updating md5.";
  //       out_db_->UpdateMD5(out_mov);
  //     }
  //   }
  //   else { // else
  //     VLOG(1) << "no images files yet.";
  //   }
  // }

  // Now two Timepoints have been created, display the elastix command
  std::string in_ref_filename = in_db_->GetImagePath(in_ref.ct_image_id);
  std::string in_mov_filename = in_db_->GetImagePath(in_mov.ct_image_id);
  DD(in_ref_filename);
  DD(in_mov_filename);
  std::string output_path = out_db_->GetRegistrationOutputPath(out_ref, out_mov);
  std::cout << "time elastix "
            << " -f " << in_ref_filename
            << " -m " << in_mov_filename
            << " -p " << config_filename_
            << " -out " << output_path << std::endl;

  // Write transformix command for DVF
  std::cout << "transformix etc ... " << output_path << std::endl;

  // Write warp command for ct and spect
  std::string in = in_db_->GetImagePath(in_mov.ct_image_id);
  std::string out = out_db_->GetImagePath(out_mov.ct_image_id);
  std::cout << "warp -r -m etc ... " << in << " " << out << " " << output_path << std::endl;

  in = in_db_->GetImagePath(in_mov.spect_image_id);
  out = out_db_->GetImagePath(out_mov.spect_image_id);
  std::cout << "warp -r -m etc ... " << in << " " << out << " " << output_path << std::endl;

}
// --------------------------------------------------------------------
