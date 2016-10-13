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
#include "sydInsertAnonymizedDicom_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydDicomSerieHelper.h"
#include "sydCommentsHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertAnonymizedDicom, 0);

  // Load plugin and get the database
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the list of dicomserie
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=0; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::DicomSerie::vector dicom_series;
  db->Query(dicom_series, ids);

  if (dicom_series.size() == 0) {
    LOG(WARNING) << "No DicomSerie found";
  }

  // Loop on dicom
  for(auto dicom:dicom_series) {
    auto ano_dicom = syd::InsertAnonymizedDicomSerie(dicom);
    syd::SetCommentsFromCommandLine(ano_dicom->comments, db, args_info);
    db->Update(ano_dicom);
    LOG(1) << "Anonymized dicom : " << ano_dicom;
    if (args_info.delete_flag) db->Delete(dicom);
  }
  // This is the end, my friend.
}
// --------------------------------------------------------------------
