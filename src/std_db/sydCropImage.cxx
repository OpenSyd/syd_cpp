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
#include "sydCropImage_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageBuilder.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT(sydCropImage, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Get the image
  syd::IdType id = atoi(args_info.inputs[1]);
  syd::Image image = db->QueryOne<syd::Image>(id);
  DD(image);
  DD(db->GetAbsolutePath(image));

  // FIXME: default is roi body, or option mask or threshold

  // Get the roi
  std::string roiname = "body";
  syd::RoiType roitype = db->FindRoiType(roiname);
  DD(roitype);
  syd::RoiMaskImage roi = db->FindRoiMaskImage(*image.patient, roitype);
  DD(roi);

  // Crop with the roi
  syd::ImageBuilder b(db);
  // FIXME no set tag here ...
  b.CropImage(image, roi);
  DD(image);

  // Update and write results


  // This is the end, my friend.
}
// --------------------------------------------------------------------
