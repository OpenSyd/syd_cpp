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
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydInsertICRPOrganDose, 3);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db =
    m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Parameters
  syd::IdType id = atoi(args_info.inputs[0]);
  auto phantom_name = args_info.phantom_arg;
  auto folder = args_info.folder_arg;

  // Get the FitTimepoint
  syd::FitTimepoints::pointer fittimepoint;
  db->QueryOne(fittimepoint, id);
  DD(fittimepoint);

  // Initialise the calculator
  auto c = std::make_shared<syd::SCoefficientCalculator>();
  c->Initialise(folder);
  c->SetPhantomName(phantom_name);

  // FIXME
  //  auto od = syd::NewOrganICRPDose(c, ft);
  //DD(od);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
