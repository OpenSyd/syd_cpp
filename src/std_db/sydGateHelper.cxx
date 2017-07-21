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
#include "sydImageHelper.h"
#include "sydTagHelper.h"

// boost
#include <boost/tokenizer.hpp>

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
syd::Image::vector syd::GateInsertOutputImages(std::string folder,
                                               syd::Image::pointer source)
{
  syd::Image::vector images;

  // Retrieve simu_name from folder must be "result.XYZ"
  auto simu_name = syd::GateGetSimulationNameFromFolder(folder);
  if (simu_name == "") return images; // bug ?

  // Clean potential already existing images
  auto db = source->GetDatabase();
  auto tag = syd::FindOrCreateTag(db, simu_name);
  auto previous_images = syd::FindImages(source->injection);
  previous_images = syd::GetRecordsThatContainTag<syd::Image>(previous_images, tag);
  db->Delete(previous_images);

  // Find all files ".mhd" in output folder
  std::vector<fs::path> files;
  std::copy(fs::directory_iterator(folder), fs::directory_iterator(), std::back_inserter(files));
  std::vector<fs::path> mhd_files;
  for(auto & file:files)
    if (file.extension() == ".mhd") mhd_files.push_back(file);

  // For each -> determine type (edep dose etc)
  for(auto & filename:mhd_files) {
    auto image = syd::GateInsertImage(fs::absolute(filename).string(), source);
    if (image != nullptr) images.push_back(image);
  }

  // Update tag
  for(auto & image:images)
    syd::AddTag(image->tags, tag);
  db->Update(images);

  /*

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


// --------------------------------------------------------------------
std::string syd::GateGetFileType(std::string filename)
{
  // Parse the filename {something}-{Dose|Edep}-{Squared|Uncertainty|} etc
  fs::path f(filename);
  f.replace_extension();
  std::string s = f.filename().string(); // (needed in tokens, cannot use .string directly)
  boost::char_separator<char> sep("-");
  boost::tokenizer< boost::char_separator<char> > tokens(s, sep);
  std::vector<std::string> words;
  for(auto t:tokens) words.push_back(t);
  bool uncertainty_flag = false;
  bool squared_flag = false;
  if (words.back() == "Uncertainty") uncertainty_flag = true;
  if (words.back() == "Squared") squared_flag = true;
  int i = words.size()-1;
  if (uncertainty_flag or squared_flag) --i;
  std::string type  ="ignored";
  if (words[i] == "Edep") type = "edep";
  else {
    if (words[i] == "Dose") type = "dose";
    else {
      LOG(WARNING) << "Ignored Gate output: " << s;
      return type;
    }
  }
  if (uncertainty_flag) type = type+"_uncertainty";
  if (squared_flag)  type = type+"_squared";
  return type;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::GateInsertImage(std::string filename,
                                         syd::Image::pointer source)
{
  // Check
  if (source->injection == nullptr) return nullptr;

  // Parse the filename {patient}-{radionuclide}-Dose etc
  auto type = syd::GateGetFileType(filename);
  if (type == "ignored") return nullptr;

  // Create an image
  auto db = source->GetDatabase<syd::StandardDatabase>();
  auto image = syd::InsertImageFromFile(filename, source->patient, type);
  image->injection = source->injection;
  image->modality = type;
  if (type.find("edep") != std::string::npos)
    image->pixel_unit = syd::FindOrCreatePixelUnit(db, "MeV");
  if (type.find("dose") != std::string::npos)
    image->pixel_unit = syd::FindOrCreatePixelUnit(db, "Gy");
  if (type.find("uncertainty") != std::string::npos)
    image->pixel_unit = syd::FindOrCreatePixelUnit(db, "%");
  image->frame_of_reference_uid = source->frame_of_reference_uid;
  image->acquisition_date = source->acquisition_date;
  image->dicoms = source->dicoms;
  db->Update(image);

  return image;
}
// --------------------------------------------------------------------
