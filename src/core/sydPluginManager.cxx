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

// std
#include <dlfcn.h>

// --------------------------------------------------------------------
syd::PluginManager * syd::PluginManager::GetInstance()
{
  if (singleton_ == NULL) {
    singleton_ = new PluginManager;
  }
  return singleton_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::PluginManager::Load()
{
  char * env = getenv ("SYD_PLUGIN");
  if (!env) {
    LOG(FATAL) << "Could not find SYD_PLUGIN. Please set this variable to the folder to look for plugins.";
  }
  LoadInFolder(std::string(env));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::PluginManager::LoadInFolder(const std::string & folder)
{
  std::string absolute_folder = folder;
  ConvertToAbsolutePath(absolute_folder);
  OFString scanPattern = "*"; // or *dcm ?
  OFString dirPrefix = "";
  OFBool recurse = OFFalse;
  size_t found=0;
  OFList<OFString> inputFiles;
  if (syd::DirExists(absolute_folder)) {
    found = OFStandard::searchDirectoryRecursively(absolute_folder.c_str(),
                                                   inputFiles, scanPattern,
                                                   dirPrefix, recurse);
    }
  else {
    LOG(FATAL) << "(plugin) The directory " << absolute_folder << " does not exist.";
  }

  for(auto f=inputFiles.begin(); f != inputFiles.end(); f++) {
    Load(f->c_str());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::PluginManager::Load(const std::string & filename)
{
  std::string ext = GetExtension(filename);
  if (ext != "so" and ext != "dylib" and ext != "DLL" and ext != "dll") {
    LOG(10) << "(plugin) ignoring the file '" << filename << "'.";
    return;
  }
  void * plugin;
  plugin = dlopen(filename.c_str(), RTLD_NOW);
  if (!plugin) {
    LOG(10) << "(plugin) The file '" << filename << "' is not a plugin.";
    return;
  }
  RegisterDatabaseSchemaFunction db_creator;
  db_creator = (RegisterDatabaseSchemaFunction) dlsym(plugin, "RegisterDatabaseSchema");
  char * result = dlerror();
  if (result) {
    LOG(10) << "(plugin) The lib " << filename << " does not contain a valid plugin for syd.";
    return;
  }
  syd::DatabaseManager * m = syd::DatabaseManager::GetInstance();
  db_creator(m);
}
// --------------------------------------------------------------------
