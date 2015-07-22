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
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydIntegratedActivityImageBuilder.h"

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  Log::SQLFlag() = false;
  Log::LogLevel() = 1;

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  syd::StandardDatabase * db;

  syd::IntegratedActivityImageBuilder integrator(db);

  auto & tac = integrator.GetTAC();

  tac.AddValue(1.0, 10);
  tac.AddValue(2.0, 20);
  // tac.AddValue(3.0, 5);
  // tac.AddValue(4.0, 3);
  DD(tac);

  double r = integrator.Integrate();
  DD(r);

  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
