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
  std::string folder = "data4";
  std::string ref_dbname = "test4_ref.db";
  std::string ref_folder = "data4_ref";

  // Get the database (copy the db3)
  auto db3 = m->Open<syd::StandardDatabase>("test3.db");
  db3->Copy(dbname, folder);
  std::cout << "Open " << dbname << " as StandardDatabase" << std::endl;
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(dbname);

  // Get a patient (with id = 1)
  syd::Patient::pointer patient;
  db->QueryOne<syd::Patient>(patient, 1); // or db->QueryOne(patient, "Patient", 1);
  DD(patient);

  // Create an image
  std::string img_filename = "input/ct_slice.mhd";
  auto image1 = syd::InsertImageFromFile(img_filename, patient);
  DD(image1);

  // Get a dicom SPECT and insert
  syd::DicomSerie::pointer dicom_serie;
  db->QueryOne(dicom_serie, 14);
  auto image2 = syd::InsertImageFromDicomSerie(dicom_serie, "float");
  DD(image2);

  // Get a dicom CT and insert
  syd::DicomSerie::pointer dicom_serie2;
  db->QueryOne(dicom_serie2, 7);
  auto image3 = syd::InsertImageFromDicomSerie(dicom_serie2, "short");
  DD(image3);

  // Update
  syd::SetPixelUnit(image1, "counts");
  syd::Injection::pointer injection;
  db->New(injection);
  std::vector<std::string> args = {"toto", "Lu-177", "2013-02-03 10:33", "188.3"};
  injection->Set(args);
  db->Insert(injection);
  syd::SetInjection(image1, "Lu-177");
  syd::AddDicomSerie(image1, dicom_serie2->id);
  db->Update(image1);
  DD(image1);

  // Scale image
  DD("Scale image");
  syd::ScaleImage(image1, 100);

  // Stitch dicom
  DD("Stitch dicom");
  syd::DicomSerie::pointer spect1, spect2;
  db->QueryOne(spect1, 14);
  db->QueryOne(spect2, 15);
  auto spect = syd::InsertStitchDicomImage(spect1, spect2, 150000, 4);
  DD(spect);

  // If needed create reference db
  if (args_info.create_ref_flag) {
    LOG(0) << "Create reference db";
    db->Copy(ref_dbname, ref_folder);
  }

  // Check
  auto ref_db = m->Open<syd::StandardDatabase>(ref_dbname);
  syd::Image::pointer ref_image;

  ref_db->QueryOne(ref_image, image1->id);
  syd::CheckSameImageAndFiles(ref_image, image1);

  ref_db->QueryOne(ref_image, image2->id);
  syd::CheckSameImageAndFiles(ref_image, image2);

  ref_db->QueryOne(ref_image, image3->id);
  syd::CheckSameImageAndFiles(ref_image, image3);

  ref_db->QueryOne(ref_image, spect->id);
  syd::CheckSameImageAndFiles(ref_image, spect);

  std::cout << "Success." << std::endl;
  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
