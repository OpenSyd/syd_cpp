
#include "extendedDatabase.h"

// --------------------------------------------------------------------
extern "C"
{
  void RegisterDatabaseSchema(syd::DatabaseManager * m) {
    auto *c = m->RegisterDatabaseSchema<ext::extendedDatabase>("extendedDB");
    c->AddSchemaName("extendedStandardSchema");
  }
}
// --------------------------------------------------------------------
