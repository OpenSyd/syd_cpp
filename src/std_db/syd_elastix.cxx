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
#include "syd_elastix_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"
#include "sydElastixHelper.h"
#include "sydFileHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(syd_elastix, 0);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Read ids from the command line and the pipe
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids); // Read the standard input if pipe
  for(auto i=0; i<args_info.inputs_num; i++)
    ids.push_back(atoi(args_info.inputs[i]));
  if (ids.size() < 1) {
    LOG(FATAL) << "Please provide at least one Elastix record id.";
  }

  // Get the Elasitx objectfs
  syd::Elastix::vector elastixs;
  db->Query(elastixs, ids);

  // Loop and perform command line
  for(auto e:elastixs) {
    // test if a current result already exist
    if (e->transform_file) {
      LOG(WARNING) << "A result transform file already exist ("
                   << e->transform_file->GetAbsolutePath()
                   << "). It will be overwritten";
    }

    auto output_dir = db->GetUniqueTempFilename("");
    fs::create_directories(output_dir);

    // Create command line
    std::ostringstream cmd;
    cmd << "elastix -f " << e->fixed_image->GetAbsolutePath()
        << " -m " << e->moving_image->GetAbsolutePath()
        << " -out " << output_dir
        << " -p " << e->config_file->GetAbsolutePath();

    // Execute the cmd line
    LOG(1) << cmd.str();
    int r = syd::ExecuteCommandLine(cmd.str(), args_info.verbose_arg);

    // test return
    std::string filename = e->ComputeDefaultFilename();//"TransformParameters.0.txt";
    if (r!=0) { // fail
      LOG(WARNING) << "Command elastix fail";
      fs::remove(output_dir);
    }
    else  {
      std::string result_file = output_dir+PATH_SEPARATOR+filename;
      if (!fs::exists(result_file)) {
        LOG(WARNING) << "Error could not find the output file: " << result_file;
        fs::remove(output_dir);
      }
      else { // only create the files if ok
        if (!e->transform_file) {
          auto folder = e->ComputeDefaultFolder();
          e->transform_file = syd::NewFile(db, folder, filename);
          db->Insert(e->transform_file);
        }
        fs::copy_file(result_file,
                      e->transform_file->GetAbsolutePath(),
                      fs::copy_option::overwrite_if_exists);
        db->Update(e);
        LOG(1) << "Registration computed. Result: " << e;
      }
    }
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
