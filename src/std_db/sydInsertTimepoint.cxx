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
#include "sydInsertTimepoint_ggo.h"
#include "sydStandardDatabase.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydTimepointBuilder.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT(sydInsertTimepoint, 3);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Get the tag
  syd::Tag tag = db->QueryOne<syd::Tag>(odb::query<syd::Tag>::label == args_info.inputs[1]);

  // Get the list of dicoms ids
  std::vector<syd::DicomSerie> dicoms;
  for(auto i=2; i<args_info.inputs_num; i++) {
    syd::IdType id = atoi(args_info.inputs[i]);
    if (db->IfExist<syd::DicomSerie>(id)) {
      syd::DicomSerie d = db->QueryOne<syd::DicomSerie>(id);
      dicoms.push_back(d);
    }
    else {
      LOG(WARNING) << "Could not find the DicomSerie with id." << id;
    }
  }

  // Insert all the dicoms. The builder guess if this is a new timepoint or not.
  syd::TimepointBuilder b(db);
  b.SetTag(tag);
  for(auto d:dicoms) {
    b.InsertDicomSerie(d);
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
