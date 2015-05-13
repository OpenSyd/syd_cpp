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

#include "sydDatabaseFactory.h"

// --------------------------------------------------------------------
// static function
void syd::DatabaseFactory::ReadDatabaseFilenames(std::string init_filename)
{
  static bool already_open=false;

  if (already_open) return;

  // Get the init filename that contains the list of db names/folders
  if (!syd::FileExists(init_filename)) {
    char * init = getenv ("SYD_INIT_DB_FILE");
    if (init == NULL) LOG(FATAL) << "Error, please set SYD_INIT_DB_FILE environment variable.";
    init_filename = std::string(init);
  }

  // Empty the maps
  map_of_database_types_.clear();
  map_of_database_param_.clear();

  // Read the file
  std::ifstream in(init_filename);
  std::string tag;
  syd::SkipComment(in);
  in >> tag;
  while (in) {
    if (tag != "<Database>") {
      LOG(FATAL) << "Error while reading the file of db (" << init_filename
                 <<"), the tag <Database> is missing ? (I read " << tag << " instead).";
    }
    std::string type;
    std::string name;
    syd::SkipComment(in); in >> type;
    syd::SkipComment(in); in >> name;
    std::string param;
    std::string s;
    syd::SkipComment(in); in >> s;
    while (in and s != "<Database>") {
      if (param == "") param = s;
      else param = param+";"+s;
      syd::SkipComment(in); in >> s;
    }
    map_of_database_types_[name] = type;
    map_of_database_param_[name] = param;
  }
  in.close();

  already_open = true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DatabaseFactory::SearchTypeAndParamFromName(std::string name,
                                                      std::string & type_name,
                                                      std::string & param)
{
  if (map_of_database_types_.find(name) == map_of_database_types_.end()) {
    std::string s;
    for(auto i : map_of_database_types_) s = s+i.first+" ";
    LOG(FATAL) << "Error could not find database named '"
               << name << "' in the list of databases. The db contains : " << s;
  }
  type_name = map_of_database_types_[name];
  param = map_of_database_param_[name];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// Static function
std::shared_ptr<syd::Database>
syd::DatabaseFactory::NewDatabase(std::string type_name,
                                  std::string name,
                                  std::string param) {
  Database * instance = nullptr;
  // find name in the registry and call factory method.
  auto it = factoryFunctionRegistry.find(type_name);

  if (it != factoryFunctionRegistry.end())
    instance = it->second(name, param); // Construct the database
  else {
    std::string s;
    for(auto i:factoryFunctionRegistry) s=s+i.first+" ";
    LOG(FATAL) << "No database type named '" << type_name
               << " in the list of known database types (" << s << ")";
  }

  // wrap instance in a shared ptr and return
  if(instance != nullptr)
    return std::shared_ptr<syd::Database>(instance);
  else
    return nullptr;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DatabaseFactory::RegisterFactoryFunction(std::string type_name, FunctionType classFactoryFunction)
{
  // register the class factory function
  factoryFunctionRegistry[type_name] = classFactoryFunction;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// Static function
syd::DatabaseFactory * syd::DatabaseFactory::GetInstance()
{
  static syd::DatabaseFactory factory;
  return &factory;
}
// --------------------------------------------------------------------
