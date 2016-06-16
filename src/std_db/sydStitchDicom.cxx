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
#include "sydStitchDicom_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStitchDicomImageBuilder.h"
#include "sydCommonGengetopt.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydStitchDicom, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the dicom series to stitch
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=0; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::DicomSerie::vector dicoms;
  db->Query(dicoms, ids);

  // Make pair, group by ids with the same dicom_frame_of_reference_uid
  std::vector<std::pair<syd::DicomSerie::pointer, syd::DicomSerie::pointer>> pairs;
  while (dicoms.size() > 0) {
    syd::DicomSerie::pointer d = dicoms.back();
    dicoms.pop_back();
    std::string n = d->dicom_frame_of_reference_uid;
    bool found = false;
    int j=0;
    while (j<dicoms.size() and !found) {
      if (dicoms[j]->dicom_frame_of_reference_uid == n) {
        syd::DicomSerie::pointer d2 = dicoms[j];
        dicoms.erase(dicoms.begin()+j);
        pairs.push_back(std::make_pair(d,d2));
        found = true;
      }
      ++j;
    }
    if (!found) {
      LOG(1) << "Dicom " << d->id << " ignored (cannot find pair dicom with same frame_of_reference_uid).";
    }
  }

  // Build stitched images
  syd::StitchDicomImageBuilder builder(db);
  for(auto p:pairs) {
    LOG(2) << "Stitching dicoms:" << std::endl
           << p.first << std::endl
           << p.second << std::endl;
    syd::Image::pointer image = builder.NewStitchedImage(p.first, p.second);
    db->UpdateTagsFromCommandLine(image->tags, args_info);
    builder.InsertAndRename(image);
    LOG(1) << "Inserting Image " << image;
  }
  //  db->Insert(images);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
