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
#include "syd_ggo.h"
#include "sydCommon.h"
#include "sydClinicalTrialDatabase.h"
#include "sydInsertDicomCommand.h"

// easylogging : only once initialization (in the main)
_INITIALIZE_EASYLOGGINGPP

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(syd, args_info);

  // Init logging option (verbose)
  syd::InitVerboseOptions(args_info);

  // Open the DBs. Use options or env variable to select the correct dbs
  syd::ClinicalTrialDatabase db;
  db.OpenDatabase();
  // syd::DBRegistration dbr;
  // dbr.OpenDatabase();
  // syd::DBAnalysis dba;
  // dba.OpenDatabase();

  // The options in the ggo group are considered in mutual exclusion, so a
  // single command will be performed.
  syd::DatabaseCommand * c;
  if (args_info.InsertDicom_given) c = new syd::InsertDicomCommand;
  // if (args_info.AAA_given) c = syd::GetDatabaseCommand("AAA");
  // if (args_info.AAA_given) c = syd::GetDatabaseCommand("AAA");
  // if (args_info.AAA_given) c = syd::GetDatabaseCommand("AAA");

  // Perform the command
  c->AddDatabase(&db);
  // c->AddDatabase(dbr);
  // c->AddDatabase(dba);
  c->SetArgs(args_info.inputs, args_info.inputs_num);
  c->Run();

}
// --------------------------------------------------------------------
