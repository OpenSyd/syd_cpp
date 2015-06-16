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
#include "sydFindDicom_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydStandardDatabase.h"

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT(sydFindDicom, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Get the patients
  std::vector<syd::Patient> patients;
  db->FindPatients(patients, args_info.inputs[1]);

  // Prepare the list of arguments
  std::vector<std::string> patterns;
  for(auto i=2; i<args_info.inputs_num; i++)
    patterns.push_back(args_info.inputs[i]);


  // Loop on patients
  std::vector<std::vector<syd::DicomSerie>> results;
   int n=0;
   for(auto patient:patients) {
    // Find
    std::vector<syd::DicomSerie> series;
    db->FindDicoms(series, patient, patterns);
    results.push_back(series);
      n+=series.size();
  }

  // Print list of ids
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
    table.AddColumn("mod", 4);
    table.AddColumn("recon", 18);
    table.AddColumn("inj",12);
    table.AddColumn("size",12);
    table.AddColumn("desc",110);
    table.Init();

     for(auto i=0; i<patients.size(); i++) {
      syd::Patient patient = patients[i];
      std::vector<syd::DicomSerie> & series = results[i];
      if (series.size() ==0) continue;

      // Dump all information
      std::string previous = series[0].acquisition_date;
      double max_time_diff = args_info.max_diff_arg;
      for(auto s:series) {
        double diff = syd::DateDifferenceInHours(s.acquisition_date, previous);
        if (diff > max_time_diff) table.SkipLine();
        table << s.id;
        if (patients.size() > 1) table << patient.name;
        table << s.acquisition_date
              << s.dicom_modality
              << s.reconstruction_date
              << s.injection->radionuclide->name
              << syd::ArrayToString<int, 3>(s.size)
              << s.dicom_description;
        previous = s.acquisition_date;
      }
    }
     table.Print(std::cout);
  }

  // Dump list of files
  if (Log::LogLevel() > 1) {
    for(auto i=0; i<patients.size(); i++) {
      syd::Patient patient = patients[i];
      std::vector<syd::DicomSerie> & series = results[i];
      for(auto s:series) {
        std::vector<syd::DicomFile> f;
        db->Query<syd::DicomFile>(odb::query<syd::DicomFile>::dicom_serie == s.id, f);
        std::cout << s.id << " " << db->GetAbsolutePath(*f[0].file) << " ";
        std::cout << std::endl;
      }
    }
  }
  // This is the end, my friend.
}
// --------------------------------------------------------------------
