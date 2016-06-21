

#include "sydDatabaseCreator.h"
#include "sydStandardDatabase.h"
#include "sydDatabaseManager.h"

static syd::DatabaseCreator<syd::StandardDatabase> * bidon =
  syd::DatabaseManager::GetInstance()->RegisterDatabaseSchema<syd::StandardDatabase>("StandardDatabase");
