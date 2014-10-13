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
#include "sydInsertRoiCommand.h"

// --------------------------------------------------------------------
syd::InsertRoiCommand::InsertRoiCommand(StudyDatabase * d):
  DatabaseCommand(), sdb_(d)
{
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::InsertRoiCommand::InsertRoiCommand(std::string d):
  DatabaseCommand()
{
  sdb_ = syd::Database::OpenDatabaseType<StudyDatabase>(d);
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertRoiCommand::Initialization()
{
  cdb_ = sdb_->get_clinical_database();
  set_move_flag(false);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::InsertRoiCommand::~InsertRoiCommand()
{

}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::InsertRoiCommand::Run(std::vector<std::string> & arg)
{
  std::string patient_name = arg[0];
  std::vector<Patient> patients;
  cdb_->GetPatientsByName(patient_name, patients);
  arg.erase(arg.begin());
  for(auto p:patients) Run(p, arg);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertRoiCommand::Run(const Patient & patient, const std::vector<std::string> & arg)
{
  DD(patient);
  DDS(arg);
  // parse args
  std::string filename; // FIXME change in vector
  RoiType roitype; // FIXME change in vector
  if (arg.size() == 2) { // only situation where the second arg could be a roiname
    std::string roiname = arg[0];
    filename = arg[1];
    DD(roiname);
    DD(filename);
    bool b = cdb_->GetIfExist<RoiType>(odb::query<RoiType>::name == roiname, roitype);
    if (!b) {
      LOG(FATAL) << "Could not find the roi type '" << roiname << "' into the db.";
    }
  }
  else {
    DD("TODO");
    // FIXME find roitype from filename.
    exit(0);
  }

  // at the end = patient mhdfilename roitype (tp ?) => RoiMaskImage
  // Get the first timepoint
  Timepoint timepoint;
  bool b = sdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::patient_id == patient.id and
                                       odb::query<Timepoint>::number == 1, timepoint);
  if (!b) {
    LOG(FATAL) << "Error, no timepoint number 1 for the patient " << patient.name;
  }
  Run(timepoint, roitype, filename);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertRoiCommand::Run(const Timepoint & timepoint,
                                const RoiType & roitype,
                                std::string filename)
{
  Patient patient(sdb_->GetPatient(timepoint));

  // Create roimaskimage
  RoiMaskImage roimaskimage;
  bool b = sdb_->GetIfExist<RoiMaskImage>(odb::query<RoiMaskImage>::timepoint_id == timepoint.id and
                                          odb::query<RoiMaskImage>::roitype_id == roitype.id,
                                          roimaskimage);
  if (!b) {
    VLOG(1) << "Creating new RoiMaskImage for patient " << patient.name
            << " and roitype " << roitype.name;
    roimaskimage = sdb_->NewRoiMaskImage(timepoint, roitype);
  }
  else {
    VLOG(1) << "Updating RoiMaskImage for patient " << patient.name
            << " and roitype " << roitype.name << " (previous file is "
            << sdb_->GetImagePath(roimaskimage);
  }
  // Update roimaskimage
  sdb_->UpdateRoiMaskImage(roimaskimage);

  // Mv or copy the filename
  std::string newfilename = sdb_->GetImagePath(roimaskimage);
  DD(get_move_flag());
  syd::RenameOrCopyMHDImage(filename, newfilename, 2, get_move_flag());
}
// --------------------------------------------------------------------
