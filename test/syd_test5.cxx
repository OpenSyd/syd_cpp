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
#include "syd_test5_ggo.h"
#include "sydCommonGengetopt.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydStandardDatabase.h"
#include "sydRoiMaskImageHelper.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(syd_test5, 0);
  LOG(WARNING) << "Need the test4 result";

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // database names
  std::string dbname = "test5.db";
  std::string folder = "data5";
  std::string ref_dbname = "test5_ref.db";
  std::string ref_folder = "data5_ref";

  // Get the database (copy the db3)
  auto db4 = m->Open<syd::StandardDatabase>("test4.db");
  db4->Copy(dbname, folder);
  std::cout << "Open " << dbname << " as StandardDatabase" << std::endl;
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(dbname);

  // Get a patient (with id = 4, the stitched one)
  syd::Image::pointer image;
  db->QueryOne<syd::Image>(image, 4);
  std::cout << "Image: " << image << std::endl;

  // Insert a roi from a mhd file
  std::string filename = "input/liver_18.mhd";
  auto roitype = syd::FindRoiType("liver", db);
  auto mask = syd::InsertRoiMaskImageFromFile(filename, image->patient, roitype);
  mask->frame_of_reference_uid = image->frame_of_reference_uid;
  mask->CopyDicomSeries(image);
  mask->acquisition_date = image->acquisition_date;
  syd::AddTag(mask->tags, image->tags);
  db->Update(mask);
  std::cout << "Mask: " << mask << std::endl;

  // ------------------------------------------------------------------
  // If needed create reference db
  if (args_info.create_ref_flag) {
    LOG(0) << "Create reference db";
    db->Copy(ref_dbname, ref_folder);
  }

  // Check
  auto ref_db = m->Open<syd::StandardDatabase>(ref_dbname);
  syd::Image::pointer ref_image;

  ref_db->QueryOne(ref_image, mask->id);
  syd::CheckSameImageAndFiles(ref_image, mask);

  std::cout << "Success." << std::endl;
  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
