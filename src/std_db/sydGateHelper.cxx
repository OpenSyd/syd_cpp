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
std::string syd::CreateGateMacroFile(std::string mac_filename,
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
std::string syd::RunGate(std::string folder,
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
