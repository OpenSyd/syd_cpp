

#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
syd::DatabaseManager * syd::DatabaseManager::singleton_;
syd::PluginManager * syd::PluginManager::singleton_;
std::map<odb::database *, syd::Database *> syd::Database::ListOfLoadedDatabases;
