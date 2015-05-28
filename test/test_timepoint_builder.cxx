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
#include "sydTimepointBuilder.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  syd::TestInitialisation();
  syd::DatabaseManager * m = syd::DatabaseManager::GetInstance();

  // Make a copy if the initial db
  std::string init_dbname = "test.db";
  std::string dbname = "test-work.db";
  std::string ref_dbname = "test-ref-tp.db";
  std::string ref_folder = "test-ref-tp-data";
  syd::CopyFile(init_dbname, dbname);

  // Load the database (with dicom)
  LOG(1) << "Loading database";
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Get tag
  syd::Tag tag = db->QueryOne<syd::Tag>(odb::query<syd::Tag>::label == "study1");

  // Get dicoms
  std::vector<syd::DicomSerie> dicoms;
  std::vector<syd::IdType> id = {1, 2};
  for(auto i:id) {
    syd::DicomSerie d = db->QueryOne<syd::DicomSerie>(i);
    dicoms.push_back(d);
  }

  // Insert some dicoms
  syd::TimepointBuilder b(db);
  b.SetTag(tag);
  b.SetIntraTimepointMaxHourDiff(1.0);
  for(auto d:dicoms) {
    b.InsertDicomSerie(d);
  }

  // Create reference is needed
  TestCreateReferenceDB(argc, argv, db, ref_dbname, ref_folder);

  // Compare table
  syd::StandardDatabase * dbref = m->Read<syd::StandardDatabase>(ref_dbname);
  bool bc = db->TableIsEqual<syd::Timepoint>(dbref);
  if (!bc) {
    LOG(FATAL) << "Table Timepoint is different between " << dbname << " and " << ref_dbname;
  }
  LOG(0) << "Table Timepoint is ok.";

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------
