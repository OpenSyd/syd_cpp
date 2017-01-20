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
#include "sydStandardDatabaseStatus_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydImageHelper.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydStandardDatabaseStatus, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  /*
    Patient 6 jm 8: 50 DicomSeries 33 images 3 masks 34 RoiStatistics
                    DicomSerie: 23 CT 23 NM 23 OT
                    Images:
    Patient 7 br 8: 50 DicomSeries 33 images 3 masks 34 RoiStatistics
   */

  syd::Patient::vector patients;
  db->Query(patients);
  DDS(patients);
  for(auto patient:patients) {
    std::cout << patient->id << " "
              << patient->name << " "
              << patient->study_id << ": "
              << syd::QueryByPatient<syd::DicomSerie>(patient).size << " dicoms"
              << syd::QueryByPatient<syd::Image>(patient).size << " images"
              << syd::QueryByPatient<syd::RoiMaskImage>(patient).size << " rois"
              << syd::QueryByPatient<syd::RoiStatistic>(patient).size << " stats"
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
