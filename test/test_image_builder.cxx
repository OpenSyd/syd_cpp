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
#include "sydTestUtils.h"
#include "sydImageBuilder.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  syd::TestInitialisation();
  syd::DatabaseManager * m = syd::DatabaseManager::GetInstance();

  // Make a copy if the initial db
  std::string init_dbname = "data/test.db";
  std::string dbname = "data/test-work.db";
  std::string ref_dbname = "data/test-ref-im.db";
  std::string ref_folder = "test-ref-im-data";
  syd::CopyFile(init_dbname, dbname);

  // Load the database (with dicom)
  LOG(1) << "Loading database";
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Create main object
  syd::ImageBuilder b(db);
  syd::Tag tag = db->QueryOne<syd::Tag>(2);
  b.SetImageTag(tag);

  // test 1: create image from dicom
  syd::DicomSerie dicom = db->QueryOne<syd::DicomSerie>(2);
  syd::Image image = b.InsertImageFromDicomSerie(dicom);
  syd::Patient patient = *image.patient;
  LOG(1) << "Create and insert Image: " << image;

  // test 2: create image for timepoint (+link tp with images)
  // syd::Timepoint tp = db->QueryOne<syd::Timepoint>(1);
  // b.InsertImagesFromTimepoint(tp);
  // LOG(1) << "Create and insert image for timepoint " << tp;

  // test 3: create image


  // Create reference
  TestCreateReferenceDB(argc, argv, db, ref_dbname, ref_folder);

  // Compare Images
  syd::StandardDatabase * dbref = m->Read<syd::StandardDatabase>(ref_dbname);
  syd::TestTableEquality<syd::Image>(db, dbref);

  // Compare files
  std::vector<syd::Image> images;
  db->Query<syd::Image>(odb::query<syd::Image>::patient == image.patient->id, images);
  std::vector<syd::Image> ref_images;
  dbref->Query<syd::Image>(odb::query<syd::Image>::patient == image.patient->id, ref_images);
  for(auto i=0; i<images.size(); i++) {
    for(auto j=0; j<images[i].files.size(); j++) {
      syd::File f1 = *images[i].files[j];
      syd::File f2 = *images[i].files[j];
      std::string a = db->GetAbsolutePath(f1);
      std::string b = dbref->GetAbsolutePath(f2);
      bool r = syd::EqualFiles(a, b);
      if (!r) {
        LOG(FATAL) << "Error file " << a << " is different from the reference " << b;
      }
    }
  }
  LOG(0) << "The files in table Image are ok.";

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------
