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
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"
#include "sydDicomStructHelper.h"
//#include "gdcmReader.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydTest, 1);

  // Load plugin and db
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);
  // -----------------------------------------------------------------


  auto dicom_struct_id = atoi(args_info.inputs[0]);
  auto roitype_id = atoi(args_info.inputs[1]);
  auto dicom_serie_id = atoi(args_info.inputs[2]);
  DD(dicom_struct_id);
  DD(roitype_id);
  DD(dicom_serie_id);

  auto dicom_struct = db->QueryOne<syd::DicomStruct>(dicom_struct_id);
  DD(dicom_struct);
  auto roitype = db->QueryOne<syd::RoiType>(roitype_id);
  DD(roitype);

  /* LATER
  auto dicom_serie = syd::FindAssociatedDicomSerie(dicom_struct);
  DD(dicom_serie);
  auto dicom_serie = db->QueryOne<syd::DicomSerie>(dicom_serie_id);
  DD(dicom_serie);
  */
  auto image = db->QueryOne<syd::Image>(dicom_serie_id); // FIXME

  auto mask = syd::InsertRoiMaskImageFromDicomStruct(dicom_struct, roitype, image, "Liver");
  DD(mask);


  // -----------------------------------------------------------------
  DD("end");
  // This is the end, my friend.
}
// --------------------------------------------------------------------
