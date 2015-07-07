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
//#include "sydImageBuilder.h"
#include "sydImageFromDicomBuilder.h"
//#include "sydTableTag.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT(sydInsertImageFromDicom, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Get the tag
  std::string tagname = args_info.inputs[1];
  syd::Tag::vector tags;
  db->FindTags(tags, tagname);
  DDS(tags);

  // Get the list of dicomserie
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=2; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::DicomSerie::vector dicom_series;
  db->Query(dicom_series, ids);

  // Create main builder
  syd::ImageFromDicomBuilder builder(db);
  syd::Image::vector images;
  for(auto d:dicom_series) {
    syd::Image::pointer image = builder.CreateImageFromDicom(d);
    for(auto t:tags) image->AddTag(t);
    images.push_back(image);
    LOG(1) << "Inserting Image " << image;
  }
  // Update for tag
  db->Update(images);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
