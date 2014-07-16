/*=========================================================================
  Program:   syd

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ===========================================================================**/

// syd
#include "sydCommon.h"
#include "sydTest_ggo.h"

// sqlite
#include <stdio.h>
#include <sqlite3.h>

// odb
#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/sqlite/database.hxx>

//#include "sydPatient.hxx"
#include "sydPatient-odb.hxx"

int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydTest, args_info);

  char * bdb = getenv ("SYD_DB");
  if (bdb == NULL) FATAL(std::endl << " please set SYD_DB environment variable." << std::endl);
  std::string mDatabaseFilename = std::string(bdb);

  odb::sqlite::database * db;
  try {
    db = new odb::sqlite::database (mDatabaseFilename);
  }
  catch (const odb::exception& e)
    {
      std::cerr << e.what () << std::endl;
      return 1;
    }

  typedef odb::query<Patient> query;
  typedef odb::result<Patient> result;

  {
    odb::transaction t (db->begin ());
    result r (db->query<Patient> (query::SynfrizzId.like("8")));
    for (result::iterator i (r.begin ()); i != r.end (); ++i) {
      std::cout << "Hello, patient Id=" << i->Id << " "
                << i->Name << " " << i->SynfrizzId << " "
                << i->Weight
                << std::endl;
    }
    t.commit ();
  }


  /// OLD TEST
  exit(0);

  {
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;

  rc = sqlite3_open(args_info.input_arg, &db);
  if (rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    exit(0);
  } else {
    fprintf(stderr, "Opened database successfully\n");
  }

  // Prepared Statement
  DD("Prepared Statement");
  // sqlite3_prepare_v2() and sqlite3_prepare16_v2() interfaces are recommended for all new programs.
  sqlite3_stmt *ppStmt;  // OUT: Statement handle
  const char *pzTail;   //  OUT: Pointer to unused portion of zSql
  /* If the nByte argument is less than zero, then zSql is read up to
     the first zero terminator. If nByte is non-negative, then it is
     the maximum number of bytes read from zSql. */
  const char * zSql = "SELECT Name FROM Patient WHERE Name='toto'";
  DD(zSql);
  int r = sqlite3_prepare_v2(db, zSql, -1, &ppStmt, &pzTail);

  if (r!= SQLITE_OK) {
    DD(" bug");
  }

  // Step
  DD("Step");
  int i=0;
  while (r = sqlite3_step(ppStmt) != SQLITE_DONE) {
    DD(i);
    if (r == SQLITE_BUSY) {
      DD("BUG busy");
    }
    const unsigned char * name =  sqlite3_column_text(ppStmt, 0); // column value
    DD(name);
    const char * t = sqlite3_column_origin_name(ppStmt, 0); // column name
    DD(t);
  }



  // Finalize
  sqlite3_close(db);
  }
}
