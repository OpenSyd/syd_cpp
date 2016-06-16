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
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "extExtendedDatabase.h"
#include "test_insert_patients.h"
#include "test_insert_injections.h"
#include "test_insert_dicoms.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  Log::SQLFlag() = false;
  Log::LogLevel() = 1;

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = "test_delete.db";
  std::string ext_dbname = "test_delete.ext.db";
  std::string folder = "test";

  //----------------------------------------------------------------
  std::cout << "Create db " << std::endl;
  m->Create("ExtendedDatabase", ext_dbname, folder, true);

  //----------------------------------------------------------------
  std::cout << "Open as ext Database" << std::endl;
  ext::ExtendedDatabase * db = m->Open<ext::ExtendedDatabase>(ext_dbname);

  //----------------------------------------------------------------
  insert_patients(db);
  insert_injections(db);

  db->Dump("Patient", "injection");
  db->Dump("Radionuclide");
  db->Dump("Injection");

  {
    syd::Injection::pointer inj;
    db->QueryOne(inj, 1);
    insert_dicoms(db, inj, "dicom");
    db->Dump("DicomSerie");
  }

  //----------------------------------------------------------------
  std::cout << std::endl << std::endl;
  syd::Injection::vector injections;
  syd::Injection::pointer inj;
  syd::Radionuclide::pointer rad;
  syd::Radionuclide::vector radionuclides;
  syd::DicomSerie::pointer serie;
  syd::DicomSerie::vector dicoms;

   {
     // Delete Serie -> will also delete DicomFile + Files
    db->QueryOne(serie, 1);
    std::cout << "Delete an dicomserie " << serie << std::endl;

    syd::DicomFile::vector dfiles;
    db->Query(dfiles);
    int total_files = dfiles.size();
    dfiles.clear();

    odb::query<syd::DicomFile> q = odb::query<syd::DicomFile>::dicom_serie == serie->id;
    db->Query(dfiles,q);
    int n = dfiles.size();
    dfiles.clear();

    db->Delete(serie);

    db->Query(dfiles);
    if (dfiles.size() != total_files-n) {
      LOG(FATAL) << "Error while deleting the serie";
    }
    else {
      std::cout << "OK, " << total_files << " before, "
                << dfiles.size() << " after" << std::endl;
    }
  }

  {
    // Delete File -> callback to erase file
    std::cout << std::endl;
    db->Dump("DicomFile");

    syd::File::vector files;
    db->Query(files);
    std::cout << "Delete a file " << files[0] << std::endl;
    db->Delete(files[0]);
    db->Dump("DicomFile");
  }

  {
    db->QueryOne(inj, 1);
    std::cout << "Delete a simple injection " << inj << std::endl;
    try {
      db->Delete(inj);
    } catch(std::exception & e) {
      std::cout << "OK deletion impossible because FK. " << e.what() << std::endl;
    }
  }

  {
    db->QueryOne(rad, 2);
    std::cout << "Delete a radionuclide on_delete(cascade)" << rad << std::endl;
    try {
      db->Delete(rad);
    } catch(std::exception & e) {
      std::cout << "OK deletion impossible because FK. " << e.what() << std::endl;
    }
  }

  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
