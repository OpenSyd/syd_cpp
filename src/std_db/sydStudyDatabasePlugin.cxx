
#include "sydStudyDatabase.h"

// --------------------------------------------------------------------
extern "C"
{
  void RegisterDatabaseSchema(syd::DatabaseManager * m) {
    auto * c = m->RegisterDatabaseSchema<syd::StudyDatabase>("StudyDB");
    c->AddSchemaName("ClinicSchema");
    c->AddSchemaName("StudySchema");
  }
}
// --------------------------------------------------------------------
