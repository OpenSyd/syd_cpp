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
#include "sydConvertDicomSerie_ggo.h"

#include "core/sydCommon.h"
#include "sydClinicDatabase.h"

// easylogging : only once initialization (in the main)
INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydConvertDicomSerie, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 3) {
    LOG(FATAL) << "Error please, provide <db> <serie> <filename>";
  }

  // Get database
  std::string dbname = args_info.inputs[0];
  std::shared_ptr<syd::ClinicDatabase> db = syd::Database::OpenDatabaseType<syd::ClinicDatabase>(dbname);

  // convert into dicom
  int serie_id = atoi(args_info.inputs[1]);
  DD(serie_id);
  std::string output_filename = args_info.inputs[2];
  DD(output_filename);

  Serie serie(db->GetById<Serie>(serie_id));
  DD(serie);
  std::string dicom_path = db->GetSeriePath(serie_id);
  DD(dicom_path);

  if (serie.modality == "CT") {
    syd::ConvertDicomCTFolderToImage(dicom_path, output_filename);
  }
  else {
    syd::ConvertDicomSPECTFileToImage(dicom_path, output_filename);
  }


  // This is the end, my friend.
}
// --------------------------------------------------------------------
