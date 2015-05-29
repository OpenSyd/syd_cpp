
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
extern "C"
{
  void RegisterDatabaseSchema(syd::DatabaseManager * m) {
    auto *c = m->RegisterDatabaseSchema<syd::StandardDatabase>("StandardDatabase");
  }
}
// --------------------------------------------------------------------
