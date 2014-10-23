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
#include "sydInsertAverageCTCommand.h"

// --------------------------------------------------------------------
syd::InsertAverageCTCommand::InsertAverageCTCommand(StudyDatabase * d):
  DatabaseCommand(), sdb_(d)
{
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::InsertAverageCTCommand::InsertAverageCTCommand(std::string d):
  DatabaseCommand()
{
  sdb_ = syd::Database::OpenDatabaseType<StudyDatabase>(d);
  Initialization();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertAverageCTCommand::Initialization()
{
  cdb_ = sdb_->get_clinical_database();
 }
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::InsertAverageCTCommand::~InsertAverageCTCommand()
{

}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::InsertAverageCTCommand::Run(std::string patient_name,
                                      std::vector<std::string> & args)
{
  std::vector<Patient> patients;
  cdb_->GetPatientsByName(patient_name, patients);
  for(auto p:patients) Run(p);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::InsertAverageCTCommand::Run(Patient & patient)
{
  RawImage average;
  bool b = sdb_->GetIfExist<RawImage>(odb::query<RawImage>::patient_id == patient.id and
                                      odb::query<RawImage>::filename == "average.mhd", average);
  if (!b) {
    VLOG(1) << "Create a new average image for patient " << patient.name;
    average = sdb_->NewRawImage(patient);
  }
  else {
    VLOG(1) << "An average image already exist for " << patient.name << " : " << average;
  }
  sdb_->UpdateAverageCTImage(average);

  // Get timepoints for this patient
  std::vector<Timepoint> timepoints;
  sdb_->LoadVector<Timepoint>(odb::query<Timepoint>::patient_id == patient.id, timepoints);

  // sort by number
  std::sort(begin(timepoints), end(timepoints),
            [&timepoints](Timepoint a, Timepoint b) { return a.number < b.number; }  );

  std::vector<RawImage> cts;
  for(auto t:timepoints) cts.push_back(sdb_->GetById<RawImage>(t.ct_image_id));
  std::vector<std::string> filenames;
  for(auto t:cts) filenames.push_back(sdb_->GetImagePath(t));

  // Create the averaged image
  typedef short PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  ImageType::Pointer image = syd::ComputeAverageImage<ImageType>(filenames);
  std::string m = syd::ComputeImageMD5<ImageType>(image);

  // Check md5 : if same, do nothing. If not same, write
  if (average.md5 != m) {
    std::string f = sdb_->GetImagePath(average);
    syd::WriteImage<ImageType>(image, f);
  }
  // Only md5 to update
  sdb_->UpdateMD5(average);
}
// --------------------------------------------------------------------
