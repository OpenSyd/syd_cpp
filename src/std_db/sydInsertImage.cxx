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
#include "sydInsertImage_ggo.h"
#include "sydStandardDatabase.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydImageBuilder.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT(sydInsertImage, 3);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Get the patient
  syd::Patient patient =
    db->FindPatientByNameOrStudyId(args_info.inputs[1]);
  DD(patient);

  // Get the dicomserie
  syd::IdType id = atoi(args_info.inputs[2]);
  syd::DicomSerie dicomserie= db->QueryOne<syd::DicomSerie>(id);
  DD(dicomserie);


  syd::Tag tag = db->QueryOne<syd::Tag>(3);
  DD(tag);

  // FIXME
  syd::ImageBuilder b(db);
  b.SetTag(tag);
  b.SetPatient(patient);

  syd::Image image = b.CreateImageFromDicomSerie(dicomserie);
  DD(image);

  // syd::Image image2 = b.CreateImageFromFile(filename);
  // DD(image2);

  //  b.InsertImage(image); // copy in the db
  //b.MoveImage(image, path, filename); -> Move File ?

  // Could fail ?
  // How to insure an image as already be created ?? md5 ? => find doublon image

  // Warning once created -> the origin of the image if lost !?  (dicom)


  // This is the end, my friend.
}
// --------------------------------------------------------------------
