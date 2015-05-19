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
#include "extTestTool_ggo.h"
#include "extMyDatabase.h"

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT(extTestTool, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  ext::MyDatabase * db = m->Read<ext::MyDatabase>(dbname);

  // Create standard file
  syd::File file;
  db->Insert(file);

  // Create extended Patien
  ext::Patient patient;
  db->Insert(patient);

  // Print content
  std::vector<ext::Patient> patients;
  db->Query(patients);
  std::cout << "There are " << patients.size() << " extPatient in the db." << std::endl;
  for(auto p:patients) std::cout << p << std::endl;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
