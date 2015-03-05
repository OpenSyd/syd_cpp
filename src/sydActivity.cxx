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
#include "core/sydCommon.h"
#include "sydActivityPeakCommand.h"
#include "sydActivityCountCommand.h"
#include "sydActivityLambdaCommand.h"
#include "sydActivityTimeIntegratedCommand.h"
#include "sydActivityDoseCommand.h"

// easylogging : only once initialization (in the main)
INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydActivity, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 2) {
    LOG(FATAL) << "Error please, provide <db> <cmd>";
  }

  // Get the current db names
  std::string db = args_info.inputs[0];

  // Add all commands managed by this tool
  std::map<std::string, syd::ActivityCommandBase*> tools;
  tools["ca"] = new syd::ActivityCountCommand;
  tools["pa"] = new syd::ActivityPeakCommand;
  tools["la"] = new syd::ActivityLambdaCommand;
  tools["ia"] = new syd::ActivityTimeIntegratedCommand;
  tools["dose"] = new syd::ActivityDoseCommand;

  // Get the command
  std::string cmd = args_info.inputs[1];
  syd::ActivityCommandBase * tool;
  auto t = tools.find(cmd);
  if (t != tools.end()) { // found
    tool = t->second;
  }
  else {
    std::string list="";
    for(auto t:tools) {
      list += " "+t.first;
    }
    LOG(FATAL) << "Error, don't know the command '" << cmd
               << "'. The current known commands are : " << list;
  }

  // Get the "free" arguments (minus db + cmd_name)
  std::vector<std::string> args;
  for(auto i=2; i<args_info.inputs_num; i++) args.push_back(args_info.inputs[i]);

  // Init the command
  std::shared_ptr<syd::ActivityDatabase> adb =
    syd::Database::OpenDatabaseType<syd::ActivityDatabase>(db);
  tool->Initialize(adb, args_info);

  // Execute the command
  tool->Run(args);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
