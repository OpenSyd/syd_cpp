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

#ifndef SYDDATABASEFACTORY_H
#define SYDDATABASEFACTORY_H

// syd
#include "sydCommon.h"
#include "sydDatabase.h"

// --------------------------------------------------------------------
namespace syd {


  // ---------------------------------------------------------------------
  class Database;
  typedef std::function<Database*(std::string, std::string)> FunctionType;
  // ---------------------------------------------------------------------


  // ---------------------------------------------------------------------
  class DatabaseFactory {
  public:

    static std::shared_ptr<Database> NewDatabase(std::string type_name,
                                                 std::string name,
                                                 std::string param);
    void RegisterFactoryFunction(std::string type_name, FunctionType classFactoryFunction);
    static DatabaseFactory * Instance();
    static std::map<std::string, FunctionType> factoryFunctionRegistry;
    static void OpenDatabaseFilenames(std::string init_filename="");
    static void GetTypeAndParamFromName(std::string name, std::string & type_name, std::string & param);
    static std::map<std::string, std::string> map_of_database_types_;
    static std::map<std::string, std::string> map_of_database_param_;
  };
  // ---------------------------------------------------------------------


  // ---------------------------------------------------------------------
  template<class T>
  class DatabaseRegistrar {
  public:
    DatabaseRegistrar(std::string type_name);
  };
  // ---------------------------------------------------------------------


#define SYD_INIT_DATABASE(DATABASE_TYPE_NAME)                         \
  virtual std::string get_typename() const { return typename_; }      \
  static std::string typename_;                                       \
  static syd::DatabaseRegistrar<DATABASE_TYPE_NAME> registrar;

#define SYD_DECLARE_DATABASE_TYPE(DATABASE_TYPE_NAME)                   \
  std::string syd::DATABASE_TYPE_NAME::typename_ = #DATABASE_TYPE_NAME; \
  syd::DatabaseRegistrar<syd::DATABASE_TYPE_NAME> syd::DATABASE_TYPE_NAME::registrar(#DATABASE_TYPE_NAME);

#include "sydDatabaseFactory.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
