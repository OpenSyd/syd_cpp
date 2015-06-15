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
#include "sydFindImage_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydStandardDatabase.h"

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT(sydFindImage, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Get the patients
  std::vector<syd::Patient> patients;
  db->FindPatients(args_info.inputs[1], patients);

  // Prepare the list of arguments
  std::vector<std::string> patterns;
  for(auto i=2; i<args_info.inputs_num; i++)
    patterns.push_back(args_info.inputs[i]);

  // Prepare the list of arguments
  std::vector<std::string> exclude;
  for(auto i=0; i<args_info.exclude_given; i++)
    exclude.push_back(args_info.exclude_arg[i]);

  // Find
  std::vector<std::vector<syd::Image>> results;
  int n=0;
  for(auto patient:patients) {
    std::vector<syd::Image> images;
    db->FindImage(patient, patterns, exclude, images); // this is sorted by acquisition date
    results.push_back(images);
    n += images.size();
  }

  // Dump list of ids
  for(auto i=0; i<patients.size(); i++)
    for(auto s:results[i]) std::cout << s.id << " ";
  if (n !=0) std::cout << std::endl;

  // Dump (if verbose)
  if (n !=0 and Log::LogLevel() > 0) {
    // init the table to output results
    syd::PrintTable table;
    table.AddColumn("#id", 4);
    if (patients.size() > 1) table.AddColumn("#p", 5);
    table.AddColumn("date", 18);
    table.AddColumn("tags", 25);
    table.AddColumn("type", 8);
    table.AddColumn("size",12);
    //    table.AddColumn("dicom",110);
    table.Init();

    for(auto i=0; i<patients.size(); i++) {
      std::vector<syd::Image> & images =results[i];
      // Dump all information
      for(auto s:images) {
        std::ostringstream size;
        size << s.size[0] << "x" << s.size[1] << "x" << s.size[2];
        table << s.id;
        if (patients.size() > 1) table << s.patient->name;
        table << s.GetAcquisitionDate()
              << GetTagLabels(s.tags)
              << s.pixel_type
              << size.str();
        // << s.dicoms[0]->dicom_description;
      }
    }
    table.Print(std::cout);
  }

  // List of files
  if (args_info.files_flag) {
    for(auto i=0; i<patients.size(); i++) {
      std::vector<syd::Image> & images =results[i];
      for(auto s:images) {
        std::cout << db->GetAbsolutePath(s) << std::endl;
      }
    }
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
