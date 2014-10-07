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
#include "sydCropCTCommand.h"

// --------------------------------------------------------------------
syd::CropCTCommand::CropCTCommand(StudyDatabase * d):
  DatabaseCommand(), sdb_(d)
{
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CropCTCommand::CropCTCommand(std::string d):
  DatabaseCommand()
{
  sdb_ = syd::Database::OpenDatabaseType<StudyDatabase>(d);
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::CropCTCommand::Initialization()
{
  cdb_ = sdb_->get_clinical_database();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CropCTCommand::~CropCTCommand()
{

}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::CropCTCommand::Run(std::string patient_name, std::string a)
{
  std::vector<Patient> patients;
  cdb_->GetPatientsByName(patient_name, patients);

  for(auto p:patients) {
    std::vector<Timepoint> timepoints;
    IdType pid = p.id;
    if (a == "all") {
      DD("todo");//      FIXME
    }
    else {
      int n = atoi(a.c_str());
      Timepoint timepoint;
      bool b = sdb_->GetIfExist<Timepoint>(odb::query<Timepoint>::patient_id == pid &&
                                           odb::query<Timepoint>::number == n, timepoint);
      if (!b) {
        LOG(WARNING) << "Error could not find number '" << n << "' for patient " << p;
      }
      else {
        timepoints.push_back(timepoint);
      }
    }
    // Loop for all number for this patient
    for(auto t:timepoints) Run(t);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::CropCTCommand::Run(const Timepoint & timepoint)
{
  // Get associated elements
  Patient patient(sdb_->GetPatient(timepoint));
  RoiType roitype(cdb_->GetRoiType("patient"));

  // Get or insert mask
  RoiMaskImage roi;
  bool b = sdb_->GetIfExist<RoiMaskImage>(odb::query<RoiMaskImage>::roitype_id == roitype.id &&
                                          odb::query<RoiMaskImage>::timepoint_id == timepoint.id, roi);
  if (!b) {
    sdb_->InsertRoiMaskImage(timepoint, roitype, roi);
    VLOG(1) << "Creating new Roi '" << roitype.name << "' for timepoint " << timepoint.number
            << " of patient " << patient.name;
  }
  else {
    VLOG(1) << "Roi " << roitype.name << " already exist for timepoint " << timepoint.number
            << " of patient " << patient.name;
  }

  sdb_->UpdateRoiMaskImage(roi);

  // Get filename
  RawImage ct(sdb_->GetById<RawImage>(timepoint.ct_image_id));
  std::string input_filename = sdb_->GetImagePath(ct);

  // Get/Create ROI output folder
  std::string output_filename = sdb_->GetImagePath(roi);

  // clitkExtractPatient : extract patient coutour
  std::cout << "clitkExtractPatient -i " << input_filename << " -o " << output_filename << " --upper=-400" << std::endl;
  //  std::cout << " (if lung does not belong to patient mask, try lower --upper)" << std::endl;

  // clitkCrop : reduce initial image size
  std::cout << "clitkCropImage -i " << input_filename << " -o " << input_filename << " --like " << output_filename << std::endl;

  // clitkSetBackground : replace air by a single -1000 value
  std::cout << "clitkSetBackground -i " << input_filename << " -o " << input_filename
            << " -m " << output_filename << " -p -1000" << std::endl;

  // Check md5 and update if needed
  if (syd::FileExists(output_filename)) {
    VLOG(1) << "Mask file exist, updating md5 " << output_filename;
    RawImage mask(sdb_->GetById<RawImage>(roi.mask_id));
    sdb_->UpdateMD5(mask);
  }
  else {
    VLOG(1) << "Still no mask file : " << output_filename;
  }
  sdb_->UpdateMD5(ct);
}
// --------------------------------------------------------------------
