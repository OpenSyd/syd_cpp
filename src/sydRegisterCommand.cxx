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
void syd::RegisterCommand::Run(std::string patient_name, const std::vector<std::string> & args)
{
  std::vector<int> numbers;
  if ((args.size() == 0) or (args[0] == "all")) {
    Patient patient;
    if (!cdb_->GetIfExist<Patient>(odb::query<Patient>::name == patient_name, patient)) {
      LOG(FATAL) << "Error, the patient " << patient_name << " does not exist";
    }
    std::vector<Timepoint> timepoints;
    in_db_->LoadVector<Timepoint>(timepoints, odb::query<Timepoint>::patient_id == patient.id);
    for(auto i:timepoints) {
      numbers.push_back(i.number);
    }
  }
  else {
    for(auto i:args) {
      numbers.push_back(atoi(i.c_str()));
    }
  }
  for(auto i:numbers) {
    if (i != 1) Run(patient_name, 1, i);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RegisterCommand::Run(std::string patient_name, int ref_number, int mov_number)
{
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
  bool b = out_db_->GetIfExist<Timepoint>(odb::query<Timepoint>::patient_id == in_ref.patient_id &&
                                          odb::query<Timepoint>::number == in_ref.number &&
                                          odb::query<Timepoint>::spect_serie_id == in_ref.spect_serie_id, out_ref);

  if (!b) { // Does not exist, create
    VLOG(1) << "Creating copy of " << in_db_->Print(in_ref);
    RawImage in_spect(in_db_->GetById<RawImage>(in_ref.spect_image_id));
    RawImage in_ct(in_db_->GetById<RawImage>(in_ref.ct_image_id));
    RawImage out_spect(in_spect);
    RawImage out_ct(in_ct);
    // out_db_->InsertTimepoint(out_ref, out_spect, out_ct);
    Serie spect_serie(cdb_->GetById<Serie>(in_ref.spect_serie_id));
    Serie ct_serie(cdb_->GetById<Serie>(in_ref.ct_serie_id));
    out_ref = out_db_->NewTimepoint(spect_serie, ct_serie);
    out_ref.copy(in_ref);
  }
  else { // already exist, check md5
    VLOG(1) << "Already existing ref timepoint, updating : " << out_db_->Print(out_ref);
  }

  // Copy files (will check md5 before copy ; will update md5 if copy)
  out_db_->CopyFilesFrom(in_db_, in_ref, out_ref);

  // Create new ot update moving tp in the new db
  Timepoint out_mov;
  b = out_db_->GetIfExist<Timepoint>(odb::query<Timepoint>::patient_id == in_mov.patient_id &&
                                     odb::query<Timepoint>::number == in_mov.number &&
                                     odb::query<Timepoint>::spect_serie_id == in_mov.spect_serie_id, out_mov);

  if (!b) { // Does not exist, create
    VLOG(1) << "Creating new moving tp of " << in_db_->Print(in_mov);
    RawImage in_spect(in_db_->GetById<RawImage>(in_mov.spect_image_id));
    RawImage in_ct(in_db_->GetById<RawImage>(in_mov.ct_image_id));
    RawImage out_spect(in_spect);
    RawImage out_ct(in_ct);
    out_spect.md5 = ""; // no image yet
    out_ct.md5 = "";    // no image yet
    //out_mov.copy(in_mov);
    //out_db_->InsertTimepoint(out_mov, out_spect, out_ct);
    Serie spect_serie(cdb_->GetById<Serie>(in_mov.spect_serie_id));
    Serie ct_serie(cdb_->GetById<Serie>(in_mov.ct_serie_id));
    out_mov = out_db_->NewTimepoint(spect_serie, ct_serie);
    out_mov.copy(in_mov);
  }

  // Now two Timepoints have been created, display the elastix command
  std::string in_ref_filename = in_db_->GetImagePath(in_ref.ct_image_id);
  std::string in_mov_filename = in_db_->GetImagePath(in_mov.ct_image_id);

  RoiMaskImage ref_roi(in_db_->GetRoiMaskImage(in_ref, "patient"));
  RoiMaskImage mov_roi(in_db_->GetRoiMaskImage(in_mov, "patient"));
  std::string in_ref_mask_filename = in_db_->GetImagePath(ref_roi);
  std::string in_mov_mask_filename = in_db_->GetImagePath(mov_roi);

  std::string output_path = out_db_->GetRegistrationOutputPath(out_ref, out_mov);
  std::cout << "time elastix "
            << " -f " << in_ref_filename
            << " -fMask " << in_ref_mask_filename
            << " -m " << in_mov_filename
            << " -mMask " << in_mov_mask_filename
            << " -p " << config_filename_
            << " -out " << output_path << std::endl;

  // Prepare param file to warp spect image
  // Load output_path TransformParameters.0.txt
  // Change (Size 480 304 798) (Spacing 1.0000000000 1.0000000000 1.0000000000) (Origin -239.5000000000 -129.5000000000 -1130.0000000000)
  // AND PIXEL TYPE ! Float and default value
  // write with another filename
  std::string param=output_path+PATH_SEPARATOR+"TransformParameters.0.txt";
  std::string param_transfo=output_path+PATH_SEPARATOR+"TransformParameters.0.spect.txt";
  RawImage in_spect(in_db_->GetById<RawImage>(in_mov.spect_image_id));
  RawImage in_ct(in_db_->GetById<RawImage>(in_mov.ct_image_id));
  {
    std::ifstream in(param);
    std::ofstream out(param_transfo);
    std::string line;
    typedef float PixelType;
    typedef itk::Image<PixelType, 3> ImageType;
    ImageType::Pointer spect = ReadImage<ImageType>(in_db_->GetImagePath(in_spect));
    while (in) {
      std::getline(in, line);
      syd::replace(line, "short", "float");
      syd::replace(line, "(DefaultPixelValue -1000.000000)", "(DefaultPixelValue 0.0)");
      if (line.find("(Size") != std::string::npos) {
        std::stringstream ss;
        ss  << "(Size " << spect->GetLargestPossibleRegion().GetSize()[0] << " "
            << spect->GetLargestPossibleRegion().GetSize()[1] << " "
            << spect->GetLargestPossibleRegion().GetSize()[2] << ")";
        line = ss.str();
      }
      if (line.find("(Spacing") != std::string::npos) {
        std::stringstream ss;
        ss  << "(Spacing " << spect->GetSpacing()[0] << " "
            << spect->GetSpacing()[1] << " "
            << spect->GetSpacing()[2] << ")";
        line = ss.str();
      }
      if (line.find("(Origin") != std::string::npos) {
        std::stringstream ss;
        ss  << "(Origin " << spect->GetOrigin()[0] << " "
            << spect->GetOrigin()[1] << " "
            << spect->GetOrigin()[2] << ")";
        line = ss.str();
      }
     out << line << std::endl;
    }
    out.close();
  }

  // Write transformix for spect
  std::string spect_folder = "TEMP_spect"+toString(in_mov.number);
  if (!syd::DirExists(spect_folder)) syd::CreateDirectory(spect_folder);
  RawImage out_spect(out_db_->GetById<RawImage>(out_mov.spect_image_id));
  std::string out_spect_filename = out_db_->GetImagePath(out_spect);
  std::cout << "transformix -out " << spect_folder << " -tp " << param_transfo << " -in " << in_db_->GetImagePath(in_spect) << std::endl;
  std::string result_spect_filename = spect_folder+"/result.mhd";
  if (syd::FileExists(result_spect_filename)) {
    syd::RenameMHDImage(result_spect_filename, out_spect_filename, 2);
    out_db_->UpdateMD5(out_spect);
  }

  // if exist copy result
  {
    std::string result_ct_filename = output_path+PATH_SEPARATOR+"result.0.mhd";
    RawImage out_ct(out_db_->GetById<RawImage>(out_mov.ct_image_id));
    std::string out_ct_filename = out_db_->GetImagePath(out_ct);
    if (syd::FileExists(result_ct_filename)) {
      syd::RenameMHDImage(result_ct_filename, out_ct_filename, 2);
      out_db_->UpdateMD5(out_ct);
    }
  }

  // Write transformix for ct
  std::string ct_folder = "TEMP_ct"+toString(in_mov.number);
  if (!syd::DirExists(ct_folder)) syd::CreateDirectory(ct_folder);
  RawImage out_ct(out_db_->GetById<RawImage>(out_mov.ct_image_id));
  std::string out_ct_filename = out_db_->GetImagePath(out_ct);
  std::cout << "transformix -out " << ct_folder << " -tp " << param << " -in " << in_db_->GetImagePath(in_ct) << std::endl;
  std::string result_ct_filename = ct_folder+"/result.mhd";
  if (syd::FileExists(result_ct_filename)) {
    syd::RenameMHDImage(result_ct_filename, out_ct_filename, 2);
    out_db_->UpdateMD5(out_ct);
  }

}
// --------------------------------------------------------------------
