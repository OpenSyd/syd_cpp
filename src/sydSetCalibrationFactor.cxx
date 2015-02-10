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
#include "sydSetCalibrationFactor_ggo.h"
#include "sydStudyDatabase.h"
#include "sydImage.h"

// easylogging : only once initialization (in the main)
INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

using namespace syd;

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydSetCalibrationFactor, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check arg
  if (args_info.inputs_num < 3) {
    LOG(FATAL) << "Error, please provide at least 3 args : <db> <patient> [<timepoint>] calib_factor";
  }

  // Open database
  std::string db_name = args_info.inputs[0];
  std::shared_ptr<StudyDatabase> sdb = Database::OpenDatabaseType<StudyDatabase>(db_name);
  std::shared_ptr<ClinicDatabase> cdb = sdb->get_clinical_database();

  // Get list of patients
  std::string patients_name = args_info.inputs[1];
  std::vector<Patient> patients;
  cdb->GetPatientsByName(patients_name, patients);

  // Get parameters
  double calibration_factor;
  std::string tp_query = "all";
  if (args_info.inputs_num > 3) {
    tp_query = args_info.inputs[2];
    calibration_factor = atof(args_info.inputs[3]);
  }
  else calibration_factor = atof(args_info.inputs[2]);

  // Loop for patient
  for(auto p:patients) {
    // Get the associated timepoints
    std::vector<Timepoint> timepoints;
    sdb->GetTimepointsByNumber(p, tp_query, timepoints);

    // Loop on timepoints and update the db
    for(auto t:timepoints) {
      t.calibration_factor = calibration_factor;
      sdb->Update<Timepoint>(t);
      ELOG(1) << "Patient " << p.name << " : change calibration factor for timepoint " << t.number
              << " (" << t.time_from_injection_in_hours  << " h) to " << calibration_factor;
    }
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
