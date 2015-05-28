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

#include "sydTestUtils.h"

//--------------------------------------------------------------------
void syd::TestInitialisation()
{
  // Log (redirect to file)
  Log::SQLFlag() = false;
  Log::LogLevel() = 10;

  std::string pwd;
  syd::GetWorkingDirectory(pwd);
  LOG(1) << "Working dir is " << pwd;

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::TestCreateReferenceDB(int argc, char* argv[],
                                syd::Database * db,
                                std::string file,
                                std::string folder)
{
  if (argc > 1) {
    if (std::string(argv[1]) == "create_ref") {
      LOG(0) << "Creating reference output...";
      db->CopyDatabaseTo(file, folder);
    }
    else {
      LOG(WARNING) << "Ignoring parameter " << argv[0];
    }
  }
}
//--------------------------------------------------------------------
