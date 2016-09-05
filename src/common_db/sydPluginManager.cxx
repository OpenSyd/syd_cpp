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
syd::PluginManager::~PluginManager()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::PluginManager * syd::PluginManager::GetInstance()
{
  // http://stackoverflow.com/questions/2505385/classes-and-static-variables-in-shared-libraries
  static syd::PluginManager * singleton_ = NULL;
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
  if (!fs::exists(folder)) {
    LOG(WARNING) << "(syd plugin) The directory "
                 << absolute_folder << " in SYD_PLUGIN does not exist.";
    return;
  }
  std::vector<std::string> files;
  SearchAndAddFilesInFolder(files, absolute_folder, false);
  for(auto f:files) {
    std::string fn=GetFilenameFromPath(f);
    if (fn != "libsydCommonDatabase.so"
        and fn != "libsydCommonDatabase.dylib"
        and fn != "libsydPlot.dylib"
        and fn != "libsydPlot.so") Load(f);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::PluginManager::ComputeDatabaseSchemaNameFromFilename(const std::string & filename) const
{
  std::string schema = filename;
  fs::path p(schema);
  schema = p.filename().string(); // get the filename, remove the path
  bool b = syd::Replace(schema, "lib", ""); // remove leading lib
  if (!b) return "";
  b = syd::Replace(schema, "Plugin.so", ""); // remove final
  if (!b) return "";
  return schema;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::PluginManager::Load(const std::string & filename)
{
  std::string schema = ComputeDatabaseSchemaNameFromFilename(filename);
  if (schema == "") {
    LOG(10) << "(syd plugin) The file '" << filename
            << "' is not in the form lib<DatabaseType>Plugin.so, ignoring.";
    return ;
  }

  // Check if this Database schema is not already registered. In this
  // case, do not load the plugin (if so, seg fault).
  auto map = syd::DatabaseManager::GetInstance()->GetRegisteredDatabaseType();
  auto it = map.find(schema);
  if (it != map.end()) {
    LOG(10) << "(syd plugin) In the file " << filename;
    LOG(10) << "(syd plugin) The database schema '"
            << schema << "' already exist, ignoring.";
    return ;
  }

  // Load the plugin
  void * plugin;
  LOG(10) << "(syd plugin) Opening the file " << filename << " to register db schema";
  plugin = dlopen(filename.c_str(), RTLD_LAZY);
  if (!plugin) {
    LOG(10) << "(syd plugin) The file '" << filename << "' is not a plugin.";
    return;
  }

  RegisterDatabaseSchemaFunction db_creator;
  db_creator = (RegisterDatabaseSchemaFunction) dlsym(plugin, "RegisterDatabaseSchema");
  char * result = dlerror();
  if (result) {
    LOG(10) << "(syd plugin) The lib " << filename << " does not contain a valid plugin for syd.";
    return;
  }
  syd::DatabaseManager * m = syd::DatabaseManager::GetInstance();
  db_creator(m);
  /*  std::string s = db_creator(m);
  if (s != schema) {
    LOG(FATAL) << "(syd plugin) The lib " << filename << " does not contain the schema '"
               << schema << "', I found '" << s << "' instead. Abort.";
  }
  */
  // end
  plugins.push_back(plugin);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::PluginManager::UnLoad()
{
  for(auto p:plugins) dlclose(p);
}
// --------------------------------------------------------------------
