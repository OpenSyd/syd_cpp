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
#include "syd_test4_ggo.h"
#include "sydCommonGengetopt.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydStandardDatabase.h"
//#include "sydImageFromDicomBuilder.h"
#include "sydTestHelper.h"
#include "sydImageHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(syd_test4, 0);
  LOG(WARNING) << "Need the test3 result";

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // database names
  std::string dbname = "test4.db";
  std::string ref_dbname = "test4_ref.db";
  std::string ref_folder = "data4_ref";

  // Get the database (copy the db3)
  auto db3 = m->Open<syd::StandardDatabase>("test3.db");
  db3->Copy(dbname);
  std::cout << "Open " << dbname << " as StandardDatabase" << std::endl;
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(dbname);

  // Get a patient (with id = 1)
  syd::Patient::pointer patient;
  db->QueryOne<syd::Patient>(patient, 1); // or db->QueryOne(patient, "Patient", 1);
  DD(patient);

  // Create an image
  std::string img_filename = "input/ct_slice.mhd";
  DD(img_filename);
  auto img = syd::InsertMhdImage(patient, img_filename, true);
  DD(img);


  DD(" STEP 2 OLD");
  /*

  // Get a dicom serie
  syd::DicomSerie::pointer dicom_serie;
  db->QueryOne(dicom_serie, 14);

  // Insert image from dicom spect
  syd::ImageFromDicomBuilder builder;
  builder.SetImagePixelType("float");
  builder.SetInputDicomSerie(dicom_serie);
  builder.Update();
  syd::Image::pointer image = builder.GetOutput();
  DD(image);

  db->QueryOne(dicom_serie, 7);
  builder.SetImagePixelType("short");
  builder.SetInputDicomSerie(dicom_serie);
  builder.Update();
  syd::Image::pointer image2 = builder.GetOutput();
  DD(image2);

  // If needed create reference db
  if (args_info.create_ref_flag) {
    LOG(0) << "Create reference db";
    db->Copy(ref_dbname, ref_folder);
  }

  // Check
  auto ref_db = m->Open<syd::StandardDatabase>(ref_dbname);
  syd::Image::pointer ref_image;
  ref_db->QueryOne(ref_image, image->id);
  syd::ImageHelper::CheckSameImageAndFiles(ref_image, image);
  ref_db->QueryOne(ref_image, image2->id);
  syd::ImageHelper::CheckSameImageAndFiles(ref_image, image2);

  */

  std::cout << "Success." << std::endl;
  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
