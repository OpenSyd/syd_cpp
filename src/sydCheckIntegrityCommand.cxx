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
#include "sydCheckIntegrityCommand.h"

// --------------------------------------------------------------------
syd::CheckIntegrityCommand::CheckIntegrityCommand(std::string db):DatabaseCommand()
{
  db_ = OpenNewDatabase<ClinicDatabase>(db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CheckIntegrityCommand::CheckIntegrityCommand(syd::ClinicDatabase * db):
  db_(db)
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CheckIntegrityCommand::~CheckIntegrityCommand()
{

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::CheckIntegrityCommand::CheckIntegrity(std::string patient_name)
{
  // Get the new patient
  Patient patient;
  if (db_->GetIfExist<Patient>(odb::query<Patient>::name == patient_name, patient)) {
    db_->CheckPatient(patient);
  }
  else {
    LOG(FATAL) << "Error, the patient " << patient_name << " does not exist";
  }

  // Part 1
  VLOG(0) << "Part 1: from DB to files";
  std::vector<Serie> series;
  db_->LoadVector<Serie>(series, odb::query<Serie>::patient_id == patient.id);
  VLOG(1) << "Found " << series.size() << " series. Checking ...";
  for(auto i=series.begin(); i<series.end(); i++) {
    VLOG(2) << "Checking serie " << i->id << " " << i->path;
    db_->CheckSerie(*i);
  }

  // Part 2
  VLOG(0) << "Part 2 : from files to DB (could be long)";
  // Search for all folders in patient folder
  std::string folder = db_->GetFullPath(patient);

  // For all folders, find dicom
  if (!OFStandard::dirExists(folder.c_str())) {
    LOG(FATAL) << "The folder " << folder << " does not exist.";
  }
  OFList<OFString> inputFiles;
  inputFiles.clear();
  OFString scanPattern = "*dcm";
  OFString dirPrefix = "";
  OFBool recurse = OFTrue;
  size_t found =
    OFStandard::searchDirectoryRecursively(folder.c_str(), inputFiles, scanPattern, dirPrefix, recurse);
  VLOG(1) << "Found " << inputFiles.size() << " files, checking...";

  // for all dicom check in db
  int n=0;
  int m = inputFiles.size()/10;
  for(auto i=inputFiles.begin(); i != inputFiles.end(); i++) {
    VLOG_EVERY_N(m, 1) << ++n << "/10";
    CheckFile(*i);
  }
  VLOG(0) << "Done, everything seems correct.";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::CheckIntegrityCommand::CheckFile(OFString filename)
{
  DcmFileFormat dfile;
  bool b = syd::OpenDicomFile(filename.c_str(), true, dfile);
  DcmObject *dset = dfile.getDataset();
  if (!b) {
    LOG(FATAL) << "Could not open the file " << filename;
  }
  std::string k;
  std::string seriesUID = GetTagValueString(dset, "SeriesInstanceUID");
  std::string SOP_UID = GetTagValueString(dset, "SOPInstanceUID");
  std::string modality = GetTagValueString(dset, "Modality");
  if (modality == "CT") k = seriesUID;
  else k = SOP_UID;

  // Find the serie
  Serie serie;
  b = db_->GetIfExist<Serie>(odb::query<Serie>::dicom_uid == k, serie);
  if (!b) {
    LOG(FATAL) << "Error the file " << filename << " contains a dicom which is not in the db.";
  }
}
// --------------------------------------------------------------------
