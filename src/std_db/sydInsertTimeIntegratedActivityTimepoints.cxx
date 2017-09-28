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
#include "sydInsertTimeIntegratedActivityTimepoints_ggo.h"
#include "sydCommonGengetopt.h"
#include "sydSCoefficientCalculator.h"
#include "sydStandardDatabase.h"
#include "sydFitImagesHelper.h"
#include "sydTimepointsHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydInsertTimeIntegratedActivityTimepoints, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db =
    m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of Timepoints
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=0; i<args_info.inputs_num; i++) {
    std::string s = args_info.inputs[i];
    auto v = std::stoi(s);
    ids.push_back(v);
  }
  syd::Timepoints::vector tps;
  db->Query(tps, ids);
  if (tps.size() == 0) {
    LOG(1) << "No Timepoints.";
    return EXIT_SUCCESS;
  }
  DDS(tps);

  /// Read the options
  syd::TimeIntegratedActivityFitOptions options;
  syd::SetOptionsFromCommandLine(options, args_info);
  DD(options);

  // Loop on all timepoints
  for(auto tp:tps) {
    DD(tp);
    options.SetLambdaDecayConstantInHours(tps[0]->injection->GetLambdaDecayConstantInHours());
    // Try to see if already exist
    auto ftps = syd::FindFitTimepoints(tp, options);
    DDS(ftps);
    if (ftps.size() == 0) {
      auto ftp = syd::NewFitTimepoints(tp, options);
      db->Insert(ftp);
      LOG(1) << "Insert FitTimepoints: " << options << " " << ftp;
    }
    if (ftps.size() == 1) {
      auto ftp = ftps[0];
      syd::ComputeFitTimepoints(ftp);
      db->Update(ftp);
      LOG(1) << "Update FitTimepoints: " << options << " " << " " << ftp;
    }
    if (ftps.size() > 1) {
      DDS(ftps);
      LOG(FATAL) << "Error several FitTimepoints with similar options exist.";
    }
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
