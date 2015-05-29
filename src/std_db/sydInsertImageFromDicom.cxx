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
#include "sydInsertImageFromDicom_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageBuilder.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT(sydInsertImageFromDicom, 3);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Get the tag
  std::string tagname = args_info.inputs[1];
  syd::Tag tag = db->QueryOne<syd::Tag>(odb::query<syd::Tag>::label == tagname);

  // Get the list of dicomserie
  std::vector<syd::DicomSerie> dicom_series;
  for(auto i=2; i<args_info.inputs_num; i++) {
    syd::IdType id = atoi(args_info.inputs[i]);
    syd::DicomSerie d= db->QueryOne<syd::DicomSerie>(id);
    dicom_series.push_back(d);
  }

  // Create main builder
  syd::ImageBuilder b(db);
  b.SetImageTag(tag);
  for(auto d:dicom_series) {
    syd::Image image = b.InsertImageFromDicomSerie(d);
    LOG(1) << "Inserting Image " << image;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
