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

#ifndef SYDDATABASECOMMAND_H
#define SYDDATABASECOMMAND_H

// syd
#include "sydDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  class DatabaseCommand
  {
  public:

    // Constructors/Destructor
    DatabaseCommand();
    ~DatabaseCommand();

    virtual void SetArgs(char ** inputs, int n) = 0;
    virtual void Run() = 0;
    void OpenDatabases();
    void CheckDatabases();

    std::string get_db_filename(std::string db);
    std::string get_db_folder(std::string db);

  protected:
    virtual void OpenCommandDatabases() = 0;
    template<class T>
    T * OpenNewDatabase(std::string name);
    std::vector<syd::Database*> databases_;
    std::map<std::string, std::string> db_filenames_;
    std::map<std::string, std::string> db_folders_;
    std::string init_filename_;
  };

#include "sydDatabaseCommand.txx"

}  // namespace syd
// --------------------------------------------------------------------

#endif // SYDDATABASECOMMAND_H
