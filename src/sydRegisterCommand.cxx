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
syd::RegisterCommand::RegisterCommand(ClinicDatabase * d1, TimepointsDatabase * d2, TimepointsDatabase * d3):
  DatabaseCommand(), db_(d1), tpdb_(d2), reg_tpdb_(d3)
{
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RegisterCommand::RegisterCommand(std::string d1, std::string d2, std::string d3):
  DatabaseCommand()
{
  db_ = OpenNewDatabase<ClinicDatabase>(d1);
  tpdb_ = OpenNewDatabase<TimepointsDatabase>(d2);
  reg_tpdb_ = OpenNewDatabase<TimepointsDatabase>(d3);
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RegisterCommand::Initialization()
{
  tpdb_->set_clinic_database(db_);
  reg_tpdb_->set_clinic_database(db_);
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
  if (!db_->GetIfExist<Patient>(odb::query<Patient>::name == patient_name, patient)) {
    LOG(FATAL) << "Error, the patient " << patient_name << " does not exist";
  }

  // Get the timepoints
  Timepoint reference_timepoint;
  bool b = tpdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::patient_id == patient.id &&
                                        odb::query<Timepoint>::number == ref_number,
                                        reference_timepoint);
  if (!b) {
    LOG(FATAL) << "Error could not find the (reference) timepoint number "
               << ref_number << " for the patient " << patient.name;
  }
  Timepoint moving_timepoint;
  b = tpdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::patient_id == patient.id &&
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
void syd::RegisterCommand::Run(Timepoint ref, Timepoint mov)
{
  // Get corresponding patient/serie
  Serie serie = db_->GetById<Serie>(mov.serie_id);
  DD(serie);

  // Check / copy for the reference image //FIXME
  Timepoint ref_timepoint;
  bool b = reg_tpdb_->GetOrInsert<Timepoint>(odb::query<Timepoint>::serie_id == ref.serie_id, ref_timepoint);

  // Update the fields
  ref_timepoint.copy(ref);
  reg_tpdb_->Update(ref_timepoint);
  // reg_tpdb_->UpdateAllTimepointNumbers(ref_timepoint.patientid);

  // Copy the ref image (if not exist)
  std::string f = reg_tpdb_->GetFullPathSPECT(ref_timepoint);
  DD(f);

  /*
  MHDImage im_in = tpdb_->GetById<MHDImage>(ref._image_spect_id);
  MHDImage im_out = reg_tpdb_->GetById<MHDImage>(ref_timepoint._image_spect_id);
  if (im_in.md5 != im_out.md5) {
    // need to copy
    DD("copy");
  }
  */

  // Verbose
  if (!b) {
    VLOG(1) << "Creating Timepoint " << reg_tpdb_->Print(ref_timepoint);
  }
  else {
    VLOG(1) << "Updating Timepoint " << reg_tpdb_->Print(ref_timepoint);
  }


  // mov timepoint
  Timepoint mov_timepoint;
  b = reg_tpdb_->GetOrInsert<Timepoint>(odb::query<Timepoint>::serie_id == mov.serie_id, mov_timepoint);

  // Update the fields
  mov_timepoint.copy(mov);
  reg_tpdb_->Update(mov_timepoint);
  // reg_tpdb_->UpdateAllTimepointNumbers(mov_timepoint.patientid);

  // Copy the mov image (if not exist)
  std::string f_mov = reg_tpdb_->GetFullPathSPECT(mov_timepoint);
  DD(f_mov);

  // Verbose
  if (!b) {
    VLOG(1) << "Creating Timepoint " << reg_tpdb_->Print(mov_timepoint);
  }
  else {
    VLOG(1) << "Updating Timepoint " << reg_tpdb_->Print(mov_timepoint);
  }

  // trial md5
  /*
  std::string s = tpdb_->GetFullPathSPECT(ref);
  DD(s);
  typedef itk::Image<float, 3> ImageType;
  ImageType::Pointer spect = syd::ReadImage<ImageType>(s);
  std::string m = md5((const char*)spect->GetBufferPointer());
  DD(m);
  */



  // Check output folder or create


  // Write elastix command
  std::string ref_filename = tpdb_->GetFullPathCT(ref);
  std::string mov_filename = tpdb_->GetFullPathCT(mov);
  DD(ref_filename);
  DD(mov_filename);

  // Write transformix command for DVF

  // Checkout image folder or create

  // Write warp command

}
// --------------------------------------------------------------------
