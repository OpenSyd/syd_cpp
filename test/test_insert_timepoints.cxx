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
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonDatabase.h"
#include "sydTimepointBuilder.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Log (redirect to file)
  Log::SQLFlag() = false;
  Log::LogLevel() = 10;

  std::string pwd;
  syd::GetWorkingDirectory(pwd);
  LOG(1) << "Working dir is " << pwd;

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager * m = syd::DatabaseManager::GetInstance();

  // Load the database (with dicom)
  LOG(1) << "Loading database";
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>("test-dicom.db");

  // Get tag
  syd::Tag tag = db->QueryOne<syd::Tag>(odb::query<syd::Tag>::label == "study3D");

  // Get dicoms
  std::vector<syd::DicomSerie> dicoms;
  std::vector<syd::IdType> id = {339, 356, 365, 376, 401, 408, 427, 434, 456, 463, 310, 311, 378, 410, 437};
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
  if (argc > 1) {
    if (std::string(argv[1]) == "create_ref") {
      LOG(0) << "Creating reference output...";
      syd::CopyFile("test-dicom.db", "test-dicom.ref.db");
    }
    else {
      LOG(WARNING) << "Ignoring parameter " << argv[0];
    }
  }

  // Compare table
  syd::Database * dbref = m->Read<syd::StandardDatabase>("test-dicom.ref.db");
  bool r = syd::CompareTable<syd::Timepoint>(db, dbref);
  if (!r) { LOG(FATAL) << "Table Timepoint is different between test-dicom.db and test-dicom.ref.db"; }
  LOG(0) << "Table Timepoint is ok.";

  // This is the end, my friend.
  return EXIT_SUCCESS;
}
// --------------------------------------------------------------------
