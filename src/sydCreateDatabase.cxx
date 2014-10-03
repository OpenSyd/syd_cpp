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
#include "sydCreateDatabase_ggo.h"
#include "core/sydCommon.h"
#include "sydClinicDatabase.h"
#include "sydStudyDatabase.h"

// easylogging : only once initialization (in the main)
_INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydCreateDatabase, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 1) {
    LOG(FATAL) << "Error please, provide <db> <cmd|patient> (see usage)";
  }

  // Get database
  std::string dbname = args_info.inputs[0];
  std::shared_ptr<syd::Database> dbg = syd::Database::OpenDatabase(dbname);

  // Generic CreateDatabase
  dbg->CreateDatabase();

  // This is the end, my friend.
}
// --------------------------------------------------------------------
