
#include "extStandardDatabase.h"

// --------------------------------------------------------------------
extern "C"
{
  void RegisterDatabaseSchema(syd::DatabaseManager * m) {
    auto *c = m->RegisterDatabaseSchema<ext::StandardDatabase>("extStandardDB");
    c->AddSchemaName("extStandardSchema");
  }
}
// --------------------------------------------------------------------
