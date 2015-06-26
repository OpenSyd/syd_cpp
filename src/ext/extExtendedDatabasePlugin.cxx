
#include "extExtendedDatabase.h"
#include "sydDatabaseManager.h"

// --------------------------------------------------------------------
extern "C"
{
  void RegisterDatabaseSchema(syd::DatabaseManager * m) {
    auto * c = m->RegisterDatabaseSchema<ext::ExtendedDatabase>("ExtendedDatabase");
    c->AddSchemaName("StandardDatabase");
  }
}
// --------------------------------------------------------------------
