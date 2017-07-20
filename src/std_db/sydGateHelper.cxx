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
#include "sydGateHelper.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
syd::GateAlias::pointer
syd::AddAlias(std::vector<GateAlias::pointer> & aliases, std::string alias_name)
{
  auto a = std::make_shared<GateAlias>();
  a->alias = alias_name;
  aliases.push_back(a);
  return a;
}// --------------------------------------------------------------------



// --------------------------------------------------------------------
std::string syd::GateCreateMacroFile(std::string mac_filename,
                                     syd::Image::pointer ct,
                                     syd::Image::pointer source,
                                     syd::Radionuclide::pointer rad,
                                     int N,
                                     std::string output)
{
  // Check ?
  // same patient ct / source
  // same frame_of_reference_uid ct / source

  // Define the known aliases
  std::vector<GateAlias::pointer> aliases;
  auto apatient = AddAlias(aliases, "PATIENT");
  auto act = AddAlias(aliases, "CT_IMAGE");
  auto asrc = AddAlias(aliases, "SOURCE_IMAGE");
  auto arad = AddAlias(aliases, "RADIONUCLIDE");
  auto az = AddAlias(aliases, "Z");
  auto aa = AddAlias(aliases, "A");
  auto an = AddAlias(aliases, "N");

  // Get output filename
  if (output == "") {
    auto db = ct->GetDatabase();
    output = db->GetUniqueTempFilename(".mac");
  }

  // Set the alias value
  act->value = ct->GetAbsolutePath();
  asrc->value = source->GetAbsolutePath();
  arad->value = rad->name;
  az->value = syd::ToString(rad->atomic_number, 0);
  aa->value = syd::ToString(rad->mass_number, 0);
  an->value = syd::ToString(N, 0);
  apatient->value = ct->patient->name;

  // Create the macro file
  std::ostringstream oss;
  oss << "# Inputs are: " << std::endl
      << "#        patient = " << apatient->value << std::endl
      << "#        source  = " << source << std::endl
      << "#                = " << source->history << std::endl
      << "#        ct      = " << ct << std::endl
      << "#                = " << ct->history << std::endl
      << "#        rad     = " << rad << std::endl;
  for(auto a:aliases) oss << a->GetMacro() << std::endl;

  std::ifstream is(mac_filename);
  oss << is.rdbuf() << std::endl;
  //std::cout << oss.str() << std::endl;

  std::ofstream of(output, std::ios::out);
  if (!of) {
    LOG(FATAL) << "Error while writing file " << output;
  }
  of << oss.str() << std::endl;
  LOG(1) << "File " << output << " written.";

  // end
  return output;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::GateRun(std::string folder,
                         std::string mac_filename,
                         int nb_thread,
                         std::string & error_output,
                         std::string & output)
{
  // Create command line
  std::ostringstream cmd;
  cmd  << "cd " << folder << " ; "
       << "pwd ; "
       << " which gate_run_submit_cluster.sh ; "
       << "gate_run_submit_cluster.sh " << mac_filename << " " << nb_thread
       << "; pwd;";

  // Execute the cmd line
  LOG(1) << cmd.str();
  int r = syd::ExecuteCommandLine(cmd.str(), 5/*not output*/,
                                  error_output, output);

  // Find the name of the simulation into the output stream
  std::string text = "runid is ";
  auto pos = output.find(text);
  std::string simu_name="";
  if (pos != std::string::npos) {
    simu_name = output.substr(pos+text.size(), 4);
  }
  else {
    LOG(WARNING) << "Cannot find the name of the simulation";
  }
  return simu_name;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::GateGetSimulationNameFromFolder(std::string folder)
{
 fs::path p(folder);
  if (!fs::is_directory(p)) return "";
  auto folder_name = p.filename().string();
  //if (folder_name == ".") folder_name = p.parent_path().string();
  auto pos = folder_name.find("results.");
  if (pos != 0) return "";
  auto simu_name = folder_name.substr(pos+8);
  return simu_name;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::vector syd::GateReadOutputImages(std::string folder, syd::Injection::pointer injection)
{
  DDF();
  syd::Image::vector images;

  // Retrieve simu_name from folder must be "result.XYZ"
  // syd::ConvertToAbsolutePath(folder);

  // DD(folder);
  auto simu_name = GateGetSimulationNameFromFolder(folder);
  DD(simu_name);

  // Find all files ".mhd" in output folder

  // For each -> determine type (edep dose etc), insert + tag

  /*
  // Loop on mhd files and txt (no folder recursive yet)
  fs::path folder(folder_name);
  if (!fs::exists(folder)) {
  LOG(FATAL) << "Folder '" << folder << "' does not exist.";
  }
  if (!fs::is_directory(folder)) {
  LOG(FATAL) << "The file '" << folder << "' is not a directory.";
  }

  std::vector<fs::path> files;
  std::copy(fs::directory_iterator(folder), fs::directory_iterator(), std::back_inserter(files));

  syd::Image::pointer tia;
  std::map<std::string, syd::Image::pointer> map_images;
  double N = 0; // number of primary
  for(auto & file:files) {
  // Insert image
  if (file.extension() == ".mhd") {
  std::string patient_name;
  std::string rad_name;
  std::string type="";

  // Parse the filename {patient}-{radionuclide}-Dose etc
  auto f(file);
  f.replace_extension();
  std::string s = f.filename().string(); // (needed in tokens, cannot use .string directly)
  boost::char_separator<char> sep("-");
  boost::tokenizer< boost::char_separator<char> > tokens(s, sep);
  std::vector<std::string> words;
  for(auto t:tokens) words.push_back(t);
  if (words.size() >= 4) {
  patient_name = words[0];
  rad_name = words[1]+"-"+words[2];
  if (words[3] == "Edep") type = "edep";
  if (words[3] == "Dose") type = "dose";
  if (words.size()>4) {
  if (words[4] == "Uncertainty") type = type+"_uncertainty";
  if (words[4] == "Squared") type = type+"_squared";
  }
  }

  if (type != "") {
  // Find image to copy info (dicom etc)
  syd::Image::vector inputs = db->FindImages(patient_name);
  std::vector<std::string> tag_names = {rad_name, study_name, "tia"};
  inputs = syd::KeepRecordIfContainsAllTags<syd::Image>(inputs, tag_names);
  if (inputs.size() < 1) {
  LOG(FATAL) << "Cannot find initial image to copy";
  }
  if (inputs.size() > 1) {
  LOG(FATAL) << inputs.size() << " images found. Try to select the one by adding tags selection.";
  }
  tia = inputs[0];

  // Check if such an image already exist
  tag_names = {rad_name, study_name, type};
  inputs = db->FindImages(patient_name);
  inputs = syd::KeepRecordIfContainsAllTags<syd::Image>(inputs, tag_names);
  if (inputs.size() != 0) {
  LOG(WARNING) << "Image " << type << " already exist for this patient/rad: " << inputs[0] << " (skip)";
  continue;
  }

  // Create image
  syd::ImageBuilder builder(db);
  syd::Image::pointer output = builder.NewMHDImageLike(tia);
  builder.CopyImageFromFile(output, file.string());

  // Set tags
  output->tags.clear(); // remove copied tags
  syd::Tag::vector tags;
  db->FindTags(tags, tag_names);
  syd::AddTag(output->tags, tags); // set default tags
  db->UpdateTagsFromCommandLine(output->tags, args_info); // user defined tags

  // Unity ? edep -> MeV ; dose -> Gy to change in "cGy/kBq.h/IA[MBq]"
  // uncer ? %
  // squared ? idem edep/dose
  syd::PixelValueUnit::pointer u;
  if (type == "dose") u = db->FindPixelUnit("Gy");
  if (type == "edep") u = db->FindPixelUnit("MeV");
  if (type == "dose_uncertainty" or type == "edep_uncertainty") u = db->FindPixelUnit("%");
  if (type == "dose_squared") u = db->FindPixelUnit("Gy");
  if (type == "edep_squared") u = db->FindPixelUnit("MeV");
  output->pixel_unit = u;

  // Insert in the db
  if (!args_info.dry_run_flag) builder.InsertAndRename(output);
  LOG(1) << "File: " << file;
  LOG(1) << "      "  << output;
  map_images[type] = output;
  }
  }
  if (file.extension() == ".txt") {
  std::ifstream f(file.string());
  std::string line;
  bool found = false;
  while (std::getline(f, line)) {
  std::istringstream iss(line);
  std::string s;
  iss >> s; // first #
  iss >> s; // second word
  if (s == "NumberOfEvents") {
  iss >> s; // read '='
  iss >> N; // read value
  found = true;
  LOG(1) << "Found nb of particules: " << N << " in " << file;
  continue;
  }
  }
  if (!found) { LOG(WARNING) << "Ignoring file " << file; }
  }
  }

  // Scale dose if needed
  if (N != 0) {
  syd::RoiStatisticBuilder rsbuilder(db);
  syd::RoiStatistic::pointer stat;
  db->New(stat);
  stat->image = tia;
  rsbuilder.ComputeStatistic(stat);// no mask
  N = N/1000.0; // 1000 because tia is in kBq.h
  double scale = (3600 * 100) * (stat->sum/N); // 100 because in cGy
  for(auto & m:map_images) {
  syd::Image::pointer image = m.second;
  std::string type = m.first;
  // Scale
  double s = scale;
  if (type == "dose_squared" or type == "edep_squared") s = scale*scale;
  if (type == "dose" or type == "edep" or
  type == "dose_squared" or type == "edep_squared") {
  syd::ScaleImageBuilder builder(db);
  builder.Scale(image, s);
  // unit
  image->pixel_unit = db->FindPixelUnit("cGy/IA[MBq]");
  // Update
  LOG(1) << "Scaling: " << image;
  if (!args_info.dry_run_flag) db->Update(image);
  }
  }
  }
  */

  return images;
}
// --------------------------------------------------------------------

