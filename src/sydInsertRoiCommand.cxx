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


template <class T> unsigned int edit_distance(const T& s1, const T& s2)
{
  const size_t len1 = s1.size(), len2 = s2.size();
  std::vector<std::vector<unsigned int> > d(len1 + 1, std::vector<unsigned int>(len2 + 1));

  d[0][0] = 0;
  for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
  for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;

  for(unsigned int i = 1; i <= len1; ++i)
    for(unsigned int j = 1; j <= len2; ++j)

      d[i][j] = std::min( std::min(d[i - 1][j] + 1,d[i][j - 1] + 1),
                          d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) );
  return d[len1][len2];
}


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
  if (arg.size() == 0) {
    LOG(FATAL) << "Error, provide the roi filename (.mhd)";
  }
  std::string filename = arg[0];
  std::string extension = syd::GetExtension(filename);
  if (extension != "mhd") {
    LOG(FATAL) << "Error the filename extension must be .mhd";
  }
  std::string roiname;
  int timepoint_number=1;
  if (arg.size() == 2) roiname = arg[1];
  if (arg.size() == 3) timepoint_number = atoi(arg[2].c_str());

  RoiType roitype;
  if (roiname == "") { // no roiname, try to guess
    std::vector<RoiType> allroitypes;
    cdb_->LoadVector<RoiType>(allroitypes);
    // get file basename
    size_t n = filename.find_last_of(PATH_SEPARATOR);
    std::string fn = filename.substr(n+1, filename.size());
    uint min = 10000;
    for(auto t:allroitypes) {
      uint d = edit_distance<std::string>(fn, t.name);
      // std::cout << fn << " " << t << " " << d << std::endl;
      if (d<min) { roitype = t; min=d; }
    }
    ELOG(2) << "Guess roitype is " << roitype.name;
  }
  else {
    bool b = cdb_->GetIfExist<RoiType>(odb::query<RoiType>::name == roiname, roitype);
    if (!b) {
      LOG(FATAL) << "Could not find the roi type '" << roiname << "' into the db.";
    }
  }

  Timepoint timepoint;
  bool b = sdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::patient_id == patient.id and
                                       odb::query<Timepoint>::number == timepoint_number, timepoint);
  if (!b) {
    LOG(FATAL) << "Error, no timepoint number " << timepoint_number << " for the patient " << patient.name;
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
    ELOG(1) << "Creating new RoiMaskImage for patient " << patient.name
            << " and roitype " << roitype.name;
    roimaskimage = sdb_->NewRoiMaskImage(timepoint, roitype);
  }
  else {
    ELOG(1) << "Updating RoiMaskImage " << roimaskimage.id << " for patient " << patient.name
            << " and roitype " << roitype.name << " (previous file is "
            << sdb_->GetImagePath(roimaskimage);
  }
  // Update roimaskimage
  sdb_->UpdateRoiMaskImage(roimaskimage);

  // Mv or copy the filename
  std::string newfilename = sdb_->GetImagePath(roimaskimage);
  syd::RenameOrCopyMHDImage(filename, newfilename, 2, get_move_flag());

  // update md5
  RawImage rawimage(sdb_->GetById<RawImage>(roimaskimage.mask_id));
  sdb_->UpdateMD5(rawimage);

  // Update roimaskimage volume/density
  sdb_->UpdateRoiMaskImageVolume(roimaskimage);
}
// --------------------------------------------------------------------
