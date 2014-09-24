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
#include "sydDatabaseCommand.h"

// --------------------------------------------------------------------
void syd::DatabaseCommand::AddDatabase(Database * db)
{
  databases_.push_back(db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DatabaseCommand::OpenDatabases()
{
  // Get the init filename that contains the list of db names/folders
  if (init_filename_ =="") { // look env var
    char * init = getenv ("SYD_INIT_DB_FILE");
    if (init == NULL) LOG(FATAL) << "Error, please set SYD_INIT_DB_FILE environment variable.";
    init_filename_ = std::string(init);
  }

  // Read the file
  std::ifstream in(init_filename_);
  while (in) {
    std::string name;
    std::string filename;
    std::string folder;
    syd::SkipComment(in);
    in >> name;
    in >> filename;
    in >> folder;
    if (in) {
      db_filenames_[name] = filename;
      db_folders_[name] = folder;
    }
  }
  in.close();

  // Call specific function to open the db
  OpenCommandDatabases();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DatabaseCommand::get_db_filename(std::string db)
{
  if (db_filenames_.find(db) != db_filenames_.end()) return db_filenames_[db];
  std::string s;
  for(auto i=db_filenames_.begin(); i != db_filenames_.end(); i++) s = s+i->first+" ";
  LOG(FATAL) << "Error could not find the database '" << db << "' in the list of databases info."
             << std::endl
             << "The db info ( " << init_filename_ << ") contains " << db_filenames_.size() << " values : " << s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DatabaseCommand::get_db_folder(std::string db)
{
  if (db_folders_.find(db) != db_folders_.end()) return db_folders_[db];
  std::string s;
  for(auto i=db_folders_.begin(); i != db_folders_.end(); i++) s = s+i->first+" ";
  LOG(FATAL) << "Error could not find the database '" << db << "' in the list of databases info."
             << std::endl
             << "The db info ( " << init_filename_ << ") contains " << db_folders_.size() << " values : " << s;
}
// --------------------------------------------------------------------
