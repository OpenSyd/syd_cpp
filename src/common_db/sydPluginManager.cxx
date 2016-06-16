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
  // http://stackoverflow.com/questions/2505385/classes-and-static-variables-in-shared-libraries
  syd::PluginManager * singleton_ = new PluginManager;
  return singleton_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::map<odb::database *, syd::Database *> &
syd::PluginManager::GetListOfLoadedDatabases()
{
  // http://stackoverflow.com/questions/2505385/classes-and-static-variables-in-shared-libraries
  static std::map<odb::database *, syd::Database *> list;
  return list;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::PluginManager::Load()
{
  char * env = getenv ("SYD_PLUGIN");
  if (!env) {
    EXCEPTION("Could not find SYD_PLUGIN. Please set this variable to the folder to look for plugins.");
  }
  std::vector<std::string> ll;
  std::string senv(env);
  std::stringstream ss (senv);
  std::string tok;
  char delimiter = ':';
  while(std::getline(ss, tok, delimiter)) {
    ll.push_back(tok);
  }
  for(auto l:ll) LoadInFolder(l);
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
  if (fs::exists(absolute_folder)) {
    found = OFStandard::searchDirectoryRecursively(absolute_folder.c_str(),
                                                   inputFiles, scanPattern,
                                                   dirPrefix, recurse);
  }
  else {
    LOG(WARNING) << "(syd plugin) The directory " << absolute_folder << " in SYD_PLUGIN does not exist.";
  }

  for(auto f=inputFiles.begin(); f != inputFiles.end(); f++) {
    std::string s(f->c_str());
    std::string fn=GetFilenameFromPath(s);
    if (fn != "libsydCommonDatabase.so"
        and fn != "libsydCommonDatabase.dylib"
        and fn != "libsydPlot.dylib") Load(s);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::PluginManager::Load(const std::string & filename)
{
  std::string ext = GetExtension(filename);
  if (ext != "so" and ext != "dylib" and ext != "DLL" and ext != "dll") {
    LOG(20) << "(syd plugin) ignoring the file '" << filename << "'.";
    return;
  }
  void * plugin;
  LOG(10) << "(syd plugin) Opening the file "
          << filename << " to register db schema";
  plugin = dlopen(filename.c_str(), RTLD_NOW);
  if (!plugin) {
    LOG(10) << "(syd plugin) The file '" << filename << "' is not a plugin.";
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
