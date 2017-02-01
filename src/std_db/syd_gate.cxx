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

class GateAlias
{
public:
  typedef std::shared_ptr<GateAlias> pointer;
  std::string alias;
  std::string value;
  std::string GetMacro() const {
    std::ostringstream oss;
    oss << "/control/alias " << alias << " " << value;
    return oss.str();
  }
};

GateAlias::pointer AddAlias(std::vector<GateAlias::pointer> & aliases, std::string alias_name) {
  auto a = std::make_shared<GateAlias>();
  a->alias = alias_name;
  aliases.push_back(a);
  return a;
}

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

  // Define the known aliases
  std::vector<GateAlias::pointer> aliases;
  auto apatient = AddAlias(aliases, "PATIENT");
  auto act = AddAlias(aliases, "CT_IMAGE");
  auto asrc = AddAlias(aliases, "SOURCE_IMAGE");
  auto arad = AddAlias(aliases, "RADIONUCLIDE");
  auto az = AddAlias(aliases, "Z");
  auto aa = AddAlias(aliases, "A");
  // other ? spacing dose or input etc

  // Fill the aliases values
  std::string mac_filename = args_info.inputs[0];
  syd::IdType image_id = atoi(args_info.inputs[1]);

  // Get input source image
  auto image = db->QueryOne<syd::Image>(image_id);
  asrc->value = image->GetAbsolutePath();

  // Find the patient
  auto patient = image->patient;
  apatient->value = patient->name;


  // -----------------------------------------------------
  // CTIMAGE -> select with tag = ct and same acqui date than the first image
  auto date = image->GetAcquisitionDate();
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
  act->value = ct->GetAbsolutePath();

  // Radionuclide
  if (image->injection == nullptr) {
    LOG(WARNING) << "No injection in the patient";
    arad->value = "Unknown";
    az->value = "Unknown";
    aa->value = "Unknown";
  }
  auto rad = image->injection->radionuclide;
  arad->value = rad->name;
  az->value = syd::ToString(rad->atomic_number, 0);
  aa->value = syd::ToString(rad->mass_number, 0);

  // Print alias (or as a file)
  std::ostringstream oss;
  oss << "# Inputs are: " << std::endl
      << "#        patient = " << patient << std::endl
      << "#        source  = " << image << std::endl
      << "#                = " << image->history << std::endl
      << "#        ct      = " << ct << std::endl
      << "#                = " << ct->history << std::endl
      << "#        rad     = " << rad << std::endl;
  for(auto a:aliases) oss << a->GetMacro() << std::endl;
  oss << "/control/execute " << mac_filename << std::endl;
  std::cout << oss.str() << std::endl;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
