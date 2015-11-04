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
#include "sydInsertRoiMaskImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydRoiMaskImageBuilder.h"
#include "sydCommonGengetopt.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertRoiMaskImage, 3);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // Get the roitype
  syd::RoiType::pointer roitype = db->FindRoiType(args_info.inputs[0]);

  // Get the dicom
  syd::IdType id = atoi(args_info.inputs[1]);
  syd::DicomSerie::pointer dicom;
  db->QueryOne(dicom, id);

  // Get mask filename
  std::string filename = args_info.inputs[2];

  // Check if already exist ?
  syd::RoiMaskImage::vector masks;
  typedef odb::query<syd::RoiMaskImage> Q;
  Q q = Q::roitype == roitype->id;
  db->Query(masks, q);
  for(auto m:masks) {
    for(auto d:m->dicoms) {
      if (d->id == dicom->id) {
        LOG(FATAL) << "A RoiMaskImage already exist with the same roitype and dicom_id. "
                   << "Remove it first if you want to replace it. " << std::endl
                   << "Existing mask is: " << m;
      }
    }
  }

  // Create a new RoiMaskImage
  syd::RoiMaskImageBuilder builder(db);
  syd::RoiMaskImage::pointer mask = builder.InsertRoiMaskImage(dicom, roitype, filename);
  db->SetImageTagsFromCommandLine(mask, args_info);

  // Final log
  LOG(1) << "Inserting RoiMaskImage " << mask;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
