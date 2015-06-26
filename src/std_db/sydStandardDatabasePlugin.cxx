
#include "sydStandardDatabase.h"
#include "sydDatabaseManager.h"

// --------------------------------------------------------------------
extern "C"
{
  void RegisterDatabaseSchema(syd::DatabaseManager * m) {
    m->RegisterDatabaseSchema<syd::StandardDatabase>("StandardDatabase");
  }
}
// --------------------------------------------------------------------
