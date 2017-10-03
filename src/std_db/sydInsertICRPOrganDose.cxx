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
#include "sydInsertICRPOrganDose_ggo.h"
#include "sydCommonGengetopt.h"
#include "sydSCoefficientCalculator.h"
#include "sydICRPOrganDoseHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydInsertICRPOrganDose, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db =
    m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of FitTimepoints (the first is target)
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=0; i<args_info.inputs_num; i++) {
    std::string s = args_info.inputs[i];
    auto v = std::stoi(s);
    ids.push_back(v);
  }
  syd::FitTimepoints::vector ftps;
  db->Query(ftps, ids);
  if (ftps.size() == 0) {
    LOG(1) << "No FitTimepoints.";
    return EXIT_SUCCESS;
  }
  if (ids.size() != ftps.size()) {
    DDS(ids);
    DDS(ftps);
    LOG(FATAL) << "Error, cannot find some FitTimepoints ids.";
  }

  // Other params
  auto phantom_name = args_info.phantom_arg;
  auto folder = args_info.folder_arg;

  // Initialise the calculator
  auto c = std::make_shared<syd::SCoefficientCalculator>();
  c->Initialise(folder);
  c->SetPhantomName(phantom_name);

  // New ICRPOrganDose
  auto od = syd::NewICRPOrganDose(c, ftps[0], ftps);
  od->md5 = od->ComputeMD5();
  odb::query<syd::ICRPOrganDose> q = odb::query<syd::ICRPOrganDose>::md5 == od->md5;
  syd::ICRPOrganDose::vector ods;
  db->Query(ods, q);
  if (ods.size() == 0) {
    db->Insert(od);
    LOG(1) << "Insert new ICRPOrganDose: " << od;
  }
  else {
    LOG(1) << "Update existing ICRPOrganDose: " << ods[0];
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
