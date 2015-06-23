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
#include "sydTest_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydStandardDatabase.h"
#include "sydDicomSerieBuilder.h"

// Init syd
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT(sydTest, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  std::string dbname = args_info.inputs[0];
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(dbname);

  // Get the patient
  std::string name = args_info.inputs[1];
  syd::Patient patient;
  syd::FindPatientByNameOrStudyId(patient, db, name);

  // Prepare the list of arguments
  std::vector<syd::DicomSerie> dicoms;
  if (args_info.inputs[2] == std::string("all")) {
    db->Query<syd::DicomSerie>(odb::query<syd::DicomSerie>::patient == patient.id, dicoms);
  }
  else {
    for(auto i=2; i<args_info.inputs_num; i++) {
      syd::DicomSerie s = db->QueryOne<syd::DicomSerie>(atoi(args_info.inputs[i]));
      dicoms.push_back(s);
    }
  }

  // Find
  syd::DicomSerieBuilder builder(db);
  std::vector<syd::DicomSerie*> toupdate;
  builder.SetInjection(*dicoms[0].injection);
  builder.SetForcePatientFlag(true);
  for(auto d:dicoms) {
    std::vector<syd::DicomFile> files;
    db->Query<syd::DicomFile>(odb::query<syd::DicomFile>::dicom_serie == d.id, files);
    // DDS(files);
    std::string filename = db->GetAbsolutePath(*files[0].file);
    DcmFileFormat dfile;
    bool b = syd::OpenDicomFile(filename, dfile);
    if (!b) {  // this is not a dicom file
      LOG(WARNING) << "Error the file '" << filename << "' is not a dicom file.";
      continue;
    }
    DcmObject * dset = dfile.getAndRemoveDataset();
    builder.UpdateDicomSerie(&d, filename, dset);

    int slice = atoi(syd::GetTagValueString(dset, "NumberOfFrames").c_str());
    if (slice != 0) d.size[2] = slice;
    else d.size[2] = db->Count<syd::DicomFile>(odb::query<syd::DicomFile>::dicom_serie == d.id);

    toupdate.push_back(new syd::DicomSerie(d));
  }
  db->Update(toupdate);
  LOG(1) << "Updated " << toupdate.size();

  // This is the end, my friend.
}
// --------------------------------------------------------------------
