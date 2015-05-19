
#include "extMyDatabase.h"

// --------------------------------------------------------------------
extern "C"
{
  void RegisterDatabaseSchema(syd::DatabaseManager * m) {
    auto *c = m->RegisterDatabaseSchema<ext::MyDatabase>("MyDatabase");
    c->AddSchemaName("StandardSchema");
    c->AddSchemaName("MySchema");
  }
}
// --------------------------------------------------------------------
