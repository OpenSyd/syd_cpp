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
#include "sydImageBuilder.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT(sydInsertRoiMaskImage, 4);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Get the roitype
  syd::RoiType roitype = db->FindRoiType(args_info.inputs[1]);

  // Get the dicom
  syd::IdType id = atoi(args_info.inputs[2]);
  syd::DicomSerie dicom = db->QueryOne<syd::DicomSerie>(id);

  // Get mask filename
  std::string filename = args_info.inputs[3];

  // FIXME DEBUG
  // Get the tag
  std::string tagname = args_info.inputs[1];
  syd::Tag tag = db->QueryOne<syd::Tag>(odb::query<syd::Tag>::label == tagname);

  LOG(FATAL) << "TODO";

  // syd::Tag tag = db->QueryOne<syd::Tag>(odb::query<syd::Tag>::label == "mask");
  // DD(tag);

  // Create main builder
  syd::ImageBuilder b(db);
  syd::RoiMaskImage mask = b.InsertRoiMaskImage(dicom, roitype, filename);
  LOG(1) << "Inserting RoiMaskImage " << mask;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
