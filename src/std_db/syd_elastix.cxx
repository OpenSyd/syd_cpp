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

#include "boost/date_time/gregorian/gregorian.hpp" //include all types plus i/o
#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::gregorian;
namespace pt = boost::posix_time;


// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT(syd_elastix, 3);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Get the images
  syd::Image::pointer fixed_image;
  db->QueryOne(fixed_image, atoi(args_info.inputs[1]));
  syd::Image::pointer moving_image;
  db->QueryOne(moving_image, atoi(args_info.inputs[2]));

  // Get the masks
  //FIXME later

  // Get the elastix config file
  std::string config_file = args_info.inputs[3];

  // Get elastix param
  std::string options = args_info.options_arg;

  // Prepare the ImageTransform to get an id
  syd::ImageTransform::pointer transfo;
  db->New(transfo);
  transfo->fixed_image = fixed_image;
  transfo->moving_image = moving_image;
  db->Insert(transfo); // insert to get id
  std::string f = syd::GetFilenameFromPath(config_file);
  std::string output_dir = transfo->ComputeRelativeFolder();
  DD(f);
  DD(output_dir);
  bool b = fs::create_directory(db->ConvertToAbsolutePath(output_dir));
  if (!b) {
    LOG(FATAL) << "Error while creating " << output_dir;
  }
  DD("output done");
  transfo->config_file = db->InsertNewFile(config_file, f, output_dir, true); // copy
  DD(transfo->config_file);
  DD("here");
  transfo->transform_file = db->InsertNewFile("", "TransformParameters.0.txt", output_dir, false); // do not copy yet

  namespace pt = boost::posix_time;
  std::ostringstream msg;
  const pt::ptime now = pt::second_clock::local_time();
  pt::time_facet*const ff = new pt::time_facet("%Y-%m-%d %H:%M:%S");
  msg.imbue(std::locale(msg.getloc(),ff));
  msg << now;
  DD(msg.str());
  transfo->date = msg.str();

  //  db->Update(transfo);

 //  transfo->date = syd::ToString(1900+timeinfo->tm_year)+"-"+
 //    timeinfo->tm_mon
 // "2015-04-01 10:00"

  // path
  std::string fixed_image_path = db->GetAbsolutePath(fixed_image);
  std::string moving_image_path = db->GetAbsolutePath(moving_image);

  DD(output_dir);

  // Create command line
  std::ostringstream cmd;
  cmd << "elastix -f " << fixed_image_path
      << " -m " << moving_image_path
      << " -out " << db->ConvertToAbsolutePath(output_dir)
      << " -p " << config_file;
  //mask here
  cmd << options; // additional options to elastix
  std::ofstream os(output_dir+PATH_SEPARATOR+"command.txt");
  os << msg.str() << std::endl
     << fixed_image->id << " " << fixed_image_path << std::endl
     << moving_image->id << " " << moving_image_path << std::endl
     << config_file << std::endl
     << cmd.str();
  os.close();

  DD(fs::file_size(config_file));

  // Execute elastix
  LOG(1) << cmd.str();
  int r = syd::ExecuteCommandLine(cmd.str(), 2); // 2 is the log level

  DD(r);

  if (r!=0) { // fail
    LOG(1) << "Command fail, removing temporary folder and table element";
    fs::remove_all(output_dir);
    db->Delete(transfo);

  }
  else  {
    DD("ok");
    std::string res = db->GetAbsolutePath(transfo->transform_file);
    DD(res);
    if (!fs::exists(res)) {
      LOG(FATAL) << "Error could not find the file " << res;
      fs::remove_all(output_dir);
      db->Delete(transfo);
    }
    db->Update(transfo);
    DD(transfo);
  }


  // Update in the db
  //  db->Update(transfo);
  //  LOG(1) << "Registration computed. Result: " << transfo;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
