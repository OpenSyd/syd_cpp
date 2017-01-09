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
#include "sydUpdate_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydUpdate, 4);

  // Get params
  std::string tablename = args_info.inputs[0];
  std::string fieldname = args_info.inputs[1];
  std::string value = args_info.inputs[2];
  syd::IdType id = atoi(args_info.inputs[3]);

  // FIXME -> allow several id at a time
  // std::vector<std::string> args;
  // for(auto i=3; i<args_info.inputs_num; i++)
  //   args.push_back(args_info.inputs[i]);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();

  // Load the database
  syd::Database * db = syd::DatabaseManager::GetInstance()->Open(args_info.db_arg);

  // Get the element
  syd::Record::pointer r;
  try {
    r = db->QueryOne(tablename, id);
  } catch(std::exception & e) {
    LOG(FATAL) << "Cannot find the element " << id << " in the table " << tablename
               << ". The error is: " << e.what();
  }

  // Update
  try {
    db->UpdateField(r, fieldname, value);
    r = db->QueryOne(tablename, id);
    LOG(1) << "Update done: " << r->ToString();
  } catch (std::exception & e) {
    LOG(FATAL) << e.what();
    /* FIXME
    syd::TableDescription * t;
    bool b = db->GetDatabaseDescription()->FindTableDescription(tablename, &t);
    if (b) LOG(FATAL) << e.what() << std::endl << "Table is " << t;
    else LOG(FATAL) << e.what() << std::endl << "Table " << tablename << " is not found.";
    */
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
