
#include "extClinicDatabase.h"

// --------------------------------------------------------------------
extern "C"
{
  void RegisterDatabaseSchema(syd::DatabaseManager * m) {
    auto *c = m->RegisterDatabaseSchema<ext::ClinicDatabase>("extClinicDB");
    c->AddSchemaName("extClinicSchema");
  }
}
// --------------------------------------------------------------------
