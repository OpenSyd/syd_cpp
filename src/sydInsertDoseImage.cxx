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
#include "sydInsertDoseImage_ggo.h"
#include "core/sydCommon.h"

// easylogging : only once initialization (in the main)
INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydInsertDoseImage, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if (args_info.inputs_num < 5) {
    cmdline_parser_sydInsertDoseImage_print_help();
    LOG(FATAL) << "Please provide at least 5 parameters";
  }

  // Get the args
  std::string db_name  = args_info.inputs[0];
  std::string patient_name  = args_info.inputs[1];
  std::string dose_filename  = args_info.inputs[2];
  std::string uncert_filename  = args_info.inputs[3];
  double scale  = atof(args_info.inputs[4]);

  // Open the db
  std::shared_ptr<syd::StudyDatabase> db =
    syd::Database::OpenDatabaseType<syd::StudyDatabase>(db_name);
  std::shared_ptr<syd::ClinicDatabase> cdb = db->get_clinical_database();

  // Go
  syd::Patient patient = cdb->GetPatientByName(patient_name);
  syd::RawImage im = db->InsertDoseImage(patient, dose_filename, uncert_filename, scale);
  ELOG(1) << "The dose '" << dose_filename << "' was inserted for patient " << patient.name;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
