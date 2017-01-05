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
#include "syd_gate_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(syd_gate, 3);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the study and patient name
  syd::IdType fit_image_id = atoi(args_info.inputs[0]);
  std::string mac_filename = args_info.inputs[1];
  int N = atoi(args_info.inputs[2]);

  // Define the alias
  std::vector<std::string> alias;

  // Get the FitImage
  syd::FitImages::pointer tia;
  db->QueryOne(tia, fit_image_id);

  // -----------------------------------------------------
  // PATIENT
  syd::Patient::pointer patient = tia->images[0]->patient;
  std::string s = "[PATIENT,"+patient->name+"]";
  alias.push_back(s);

  // -----------------------------------------------------
  // CTIMAGE -> select with tag = ct and same acqui date than the first image
  auto first_spect = tia->images[0];
  auto date = first_spect->GetAcquisitionDate();
  syd::Image::vector candidates;
  odb::query<syd::Image> q =
    odb::query<syd::Image>::patient == patient->id and
    (odb::query<syd::Image>::modality == "CT" or
     odb::query<syd::Image>::modality == "ct");
  db->Query(candidates, q);

  // remove ct too far away from the date
  auto it = std::begin(candidates);
  while (it != std::end(candidates)) {
    auto time_interval =
      fabs(syd::DateDifferenceInHours(date, (*it)->acquisition_date));
    if (time_interval> 0.5) // FIXME put an option
      candidates.erase(it);
    else ++it;
  }

  // If several CT, keep the one with the largest spacing (FIXME)
  double max_spacing = 0.0;
  int i_max = -1;
  int i = 0;
  for(auto c:candidates) {
    if (c->spacing[0]>max_spacing) {
      max_spacing = c->spacing[0];
      i_max = i;
    }
    ++i;
  }
  auto ct = candidates[i_max];

  auto path = ct->GetAbsolutePath();
  if (args_info.relative_flag) {
    fs::path p(path);
    path = "data/"+p.filename().string();
  }
  s = "[CTIMAGE,"+path+"]";
  alias.push_back(s);

  // -----------------------------------------------------
  // RADIONUCLIDE
  auto rad = first_spect->injection->radionuclide;
  s = "[RADIONUCLIDE,"+rad->name+"]";
  alias.push_back(s);
  int A = rad->mass_number;
  s = "[A,"+std::to_string(A)+"]";
  alias.push_back(s);
  int Z = rad->atomic_number;
  s = "[Z,"+std::to_string(Z)+"]";
  alias.push_back(s);

  // -----------------------------------------------------
  // SPECTIMAGE
  path = tia->GetOutput("fit_auc")->GetAbsolutePath();
  if (args_info.relative_flag) {
    fs::path p(path);
    path = "data/"+p.filename().string();
  }
  s = "[SPECTIMAGE,"+path+"]";
  alias.push_back(s);

  // -----------------------------------------------------
  // N
  s = "[N,"+std::to_string(N)+"]";
  alias.push_back(s);

  // Final print //FIXME to the same for gate_run_submit_cluster
  if (args_info.cluster_flag) {
    std::cout << "gate_run_submit_cluster " << mac_filename
              << " 2 \"\" \"-a '";
    for(auto a:alias) std::cout << a; // no space between alias
    std::cout << "'\"" << std::endl;
  }
  else {
    std::cout << "Gate " << mac_filename
              << " -a '";
    for(auto a:alias) std::cout << a << " ";
    std::cout << "'" << std::endl;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
