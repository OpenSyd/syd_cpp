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

#ifndef SYDPLUGINMANAGER_H
#define SYDPLUGINMANAGER_H

// syd
#include "sydDatabaseManager.h"

// --------------------------------------------------------------------
namespace syd {

  class PluginManager {
  public:

    /// Retrieve the unique instance (singleton)
    static PluginManager * GetInstance();

    void LoadInFolder(const std::string & folder);
    void Load(const std::string & filename);
    void Load();

  protected:
    /// Purposely protected, only a single instance possible
    PluginManager() { }
    ~PluginManager() { }

  };
} // end namespace
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/// Those functions must be included in the plugin.
typedef void (*RegisterDatabaseSchemaFunction) (syd::DatabaseManager * m);
extern void RegisterDatabaseSchema(syd::DatabaseManager * m);
// --------------------------------------------------------------------

#endif
