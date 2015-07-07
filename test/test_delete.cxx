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

// Init syd
SYD_STATIC_INIT

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
  ext::ExtendedDatabase * db = m->Read<ext::ExtendedDatabase>(ext_dbname);

  //----------------------------------------------------------------
  insert_patients(db);
  insert_injections(db);

  {
    ext::Patient::vector patients;
    db->Query(patients);
    db->Dump(patients, "injection");

    syd::Radionuclide::vector r;
    db->Query(r);
    db->Dump(r);

    syd::Injection::vector injections;
    db->Query(injections);
    db->Dump(injections);
  }

  {
    syd::Injection::pointer inj;
    db->QueryOne(inj, 1);
    insert_dicoms(db, inj, "dicom");

    syd::DicomSerie::vector dicoms;
    db->Query(dicoms);
    db->Dump(dicoms);
  }

  //----------------------------------------------------------------

  syd::Injection::vector injections;
  syd::Injection::pointer inj;
  syd::Radionuclide::pointer rad;
  syd::Radionuclide::vector radionuclides;
  syd::DicomSerie::pointer serie;
  syd::DicomSerie::vector dicoms;

  {
    db->QueryOne(serie, 2);
    DD(serie);
    db->Delete(serie);
    db->Query(dicoms);
    db->Dump(dicoms);
  }


  {
    db->QueryOne(inj, 1);
    std::cout << "Delete a simple injection " << inj << std::endl;
    db->Delete(inj); // -> nothing else deleted
    db->Query(injections);
    db->Dump(injections);
  }

  {
    db->QueryOne(rad, 2);
    std::cout << "Delete a radionuclide on_delete(cascade)" << rad << std::endl;
    db->Delete(rad); // -> one injection must be deleted
    db->Query(radionuclides);
    db->Dump(radionuclides);
    injections.clear();
    db->Query(injections);
    db->Dump(injections);
  }


  // On delete patient -> injection (not tested, like radionuclide)

  // File ? must delete file on disks

  // DicomSerie
  // DicomFile

  // Tag ? warning with images
  //

  /*
  db->Delete(rad); // -> delete injections also
  db->Delete(patient); // -> delete injections also
  */


  return EXIT_SUCCESS;
  // This is the end, my friend.
}
// --------------------------------------------------------------------
