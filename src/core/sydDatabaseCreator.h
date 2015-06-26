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

#ifndef SYDDATABASECREATOR_H
#define SYDDATABASECREATOR_H

// syd
#include "sydDatabaseManager.h"
#include "sydDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// Base classe for classes that create a Database
  class DatabaseCreatorBase {

  public:
    virtual Database * Read(std::string filename) = 0;
    virtual void Create(std::string dbtype, std::string filename, std::string folder) = 0;
    void AddSchemaName(std::string n) { schemas.push_back(n); }

  protected:
    std::vector<std::string> schemas;
  };

  /// Main class to create/open a database of a given type, with a list of schema
  template<class DatabaseSchema>
  class DatabaseCreator:public DatabaseCreatorBase {
  public:
    /// Main function to open a db ; read the file and build the tables
    virtual Database * Read(std::string filename);

    /// Main function to create the db and consider all schemas
    virtual void Create(std::string dbtype, std::string filename, std::string folder);
  };

#include "sydDatabaseCreator.txx"

} // end namespace

// --------------------------------------------------------------------

#endif
