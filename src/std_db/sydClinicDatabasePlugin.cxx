
#include "sydClinicDatabase.h"

// --------------------------------------------------------------------
extern "C"
{
  void RegisterDatabaseSchema(syd::DatabaseManager * m) {
    auto *c = m->RegisterDatabaseSchema<syd::ClinicDatabase>("ClinicDB");
    c->AddSchemaName("ClinicSchema");
  }
}
// --------------------------------------------------------------------
