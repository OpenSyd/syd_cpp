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
#include "sydDumpSeriesCommand.h"
#include "sydCheckIntegrityCommand.h"
#include "sydClearSeriesCommand.h"
#include "sydInsertTimePointCommand.h"

// easylogging : only once initialization (in the main)
INITIALIZE_EASYLOGGINGPP

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(syd, args_info);

  // Init logging option (verbose)
  syd::InitVerboseOptions(args_info);

  // Open the main DB. Use options or env variable to select the correct dbs
  syd::ClinicalTrialDatabase db;
  db.OpenDatabase();

  // The options in the ggo group are considered in mutual exclusion, so a
  // single command will be performed.
  syd::DatabaseCommand * c;
  if (args_info.InsertDicom_given) {
    syd::InsertDicomCommand * a = new syd::InsertDicomCommand;
    a->set_rename_flag(args_info.rename_flag);
    c = a;
  }
  if (args_info.ClearSeries_given) c = new syd::ClearSeriesCommand;
  if (args_info.DumpSeries_given) c = new syd::DumpSeriesCommand;
  if (args_info.CheckIntegrity_given) {
    syd::CheckIntegrityCommand * a = new syd::CheckIntegrityCommand;
    db.set_check_file_content_level(args_info.checkLevel_arg);
    c = a;
  }
  if (args_info.InsertTimePoint_given) {
    if (!args_info.tpdb_given) {
      LOG(FATAL) << "Please, set --tpdb";
    }
    if (!args_info.tpdbfolder_given) {
      LOG(FATAL) << "Please, set --tpdbfolder";
    }
    syd::InsertTimePointCommand * a = new syd::InsertTimePointCommand;
    // Open TimePoint DB if needed
    syd::TimePointsDatabase * tpdb = new syd::TimePointsDatabase;
    tpdb->OpenDatabase(std::string(args_info.tpdb_arg), std::string(args_info.tpdbfolder_arg));
    a->AddDatabase(tpdb); // order to add db is important (tpdb must be first)
    a->set_ct_selection_patterns(args_info.ct_arg);
    c = a;
  }

  // Perform the command
  c->AddDatabase(&db);
  c->SetArgs(args_info.inputs, args_info.inputs_num);
  c->Run();

  // This is the end, my friend.
}
// --------------------------------------------------------------------
