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
#include "sydImageHelper.h"
#include "sydTagHelper.h"
#include "sydCommentsHelper.h"
#include "sydCommonGengetopt.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydStitchDicom, 0);

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
  std::vector<syd::DicomSerie::vector> temp = GroupByStitchableDicom(dicoms);
  if (args_info.force_flag and dicoms.size() == 2) {
    temp.clear();
    temp.push_back(dicoms);
  }
  std::vector<syd::DicomSerie::vector> groups;
  for(auto &t:temp) {
    if (t.size() > 1) groups.push_back(t);
    else {
      LOG(1) << "Dicom " << t[0]->id << " ignored (cannot find stitchable dicom).";
    }
  }

  // Build stitched images
  for(auto & g:groups) {
    auto d1 = g[0];
    auto d2 = g[1];
    if (g.size() > 2) {
      LOG(WARNING) << "Cannot stitch more than 2 dicoms yet";
      continue;
    }
    LOG(2) << "Stitching dicoms:" << std::endl
           << d1 << std::endl
           << d2 << std::endl;
    if (args_info.dry_run_flag) continue;
    auto image = syd::InsertStitchDicomImage(d1, d2,
                                             args_info.t_cumul_arg,
                                             args_info.skip_slices_arg);
    syd::SetImageInfoFromCommandLine(image, args_info);
    syd::SetTagsFromCommandLine(image->tags, db, args_info);
    syd::SetCommentsFromCommandLine(image->comments, db, args_info);
    db->Update(image);
    LOG(1) << "Inserting Image " << image;
  }
  //  db->Insert(images);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
