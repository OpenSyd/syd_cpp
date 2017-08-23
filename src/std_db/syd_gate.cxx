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
#include "sydGateHelper.h"
#include "sydRadionuclideHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(syd_gate, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get input data
  std::string mac_filename = args_info.inputs[0];
  syd::IdType ct_id = atoi(args_info.inputs[1]);
  syd::IdType source_id = atoi(args_info.inputs[2]);
  std::string rad_name = args_info.inputs[3];
  int N = args_info.N_arg;
  std::string output_mac = args_info.output_arg;

  // Get objects
  auto ct = db->QueryOne<syd::Image>(ct_id);
  auto source = db->QueryOne<syd::Image>(source_id);
  auto rad = syd::FindRadionuclide(db, rad_name);

  // Create macro
  // Get output filename
  if (output_mac == "") output_mac = db->GetUniqueTempFilename(".mac");
  syd::GateCreateMacroFile(mac_filename, ct, source, rad, N, output_mac);

  // Run simulation
  if (args_info.run_given) {
    auto cwd = boost::filesystem::current_path();
    std::string output;
    std::string error_output;
    auto simu_name = syd::GateRun(cwd.string(), output_mac, args_info.run_arg, error_output, output);
    LOG(2) << "Simulation output " << output_mac;
    LOG(2) << "Simulation err output " << error_output;
    if (simu_name != "") {
      LOG(1) << "Simulation started: " << simu_name;
    }
    else {
      LOG(1) << "Error : no simulation started (both gate_run_submit_cluster.sh and Gate must be in PATH).";
    }
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
