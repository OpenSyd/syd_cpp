/*=========================================================================
  Program:   sfz

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
#include "sydInsertGateOutput_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydCommonGengetopt.h"
#include "sydRoiStatisticBuilder.h"
#include "sydScaleImageBuilder.h"

#include <boost/tokenizer.hpp>

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydInsertGateOutput, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // Get the study and patient name
  std::string study_name = args_info.inputs[0];
  std::string folder_name = args_info.inputs[1];

  // Loop on mhd files and txt (no folder recursive yet)
  fs::path folder(folder_name);
  DD(folder);
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
        syd::Image::vector inputs;
        std::vector<std::string> tag_names = {rad_name, study_name, "tia"};
        db->QueryByTags<syd::Image>(inputs, tag_names, patient_name);
        if (inputs.size() < 1) {
          LOG(FATAL) << "Cannot find initial image to copy";
        }
        if (inputs.size() > 1) {
          LOG(FATAL) << inputs.size() << " images found. Try to select the one by adding tags selection.";
        }
        tia = inputs[0];

        // Check if such an image already exist
        tag_names = {rad_name, study_name, type};
        db->QueryByTags<syd::Image>(inputs, tag_names, patient_name);
        if (inputs.size() != 0) {
          LOG(FATAL) << "Image " << type << " already exist for this patient/rad: " << inputs[0];
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
        if (type == "dose") u = db->FindPixelValueUnit("Gy");
        if (type == "edep") u = db->FindPixelValueUnit("MeV");
        if (type == "dose_uncertainty" or type == "edep_uncertainty") u = db->FindPixelValueUnit("%");
        if (type == "dose_squared") u = db->FindPixelValueUnit("Gy");
        if (type == "edep_squared") u = db->FindPixelValueUnit("MeV");
        output->pixel_value_unit = u;

        // Insert in the db
        builder.InsertAndRename(output);
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
  syd::RoiStatisticBuilder rsbuilder(db);
  syd::RoiStatistic::pointer stat;
  db->New(stat);
  stat->image = tia;
  rsbuilder.ComputeStatistic(stat);// no mask
  double scale = (stat->sum / N / 1000.0);
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
      image->pixel_value_unit = db->FindPixelValueUnit("cGy/IA[MBq]");
      // Update
      LOG(1) << "Scaling: " << image;
      db->Update(image);
    }
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
