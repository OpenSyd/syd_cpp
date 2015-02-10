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
#include "sydTestCalibration_ggo.h"
#include "sydCommon.h"
#include "sydClinicDatabase.h"
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
  GGO(sydTestCalibration, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check arg
  if (args_info.inputs_num < 2) {
    LOG(FATAL) << "Error, please provide at least 2 args : <db> <patient>";
  }

  // Open database
  std::string db_name = args_info.inputs[0];
  std::shared_ptr<StudyDatabase> sdb = Database::OpenDatabaseType<StudyDatabase>(db_name);
  std::shared_ptr<ClinicDatabase> cdb = sdb->get_clinical_database();

  // Get list of patients
  std::string patients_name = args_info.inputs[1];
  std::vector<Patient> patients;
  cdb->GetPatientsByName(patients_name, patients);

  // Prepare table for output
  syd::PrintTable table;
  table.AddColumn("#p", 5, 0);
  table.AddColumn("p", 10, 0);
  table.AddColumn("IA(MBq)", 10, 1);
  table.AddColumn("t1", 10, 1);
  table.AddColumn("IA-t1", 10, 1);
  table.AddColumn("counts", 15, 0);
  table.AddColumn("ratio", 15, 0);
  table.Init();

  // Image type
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;

  // for statistics
  double mean_ratio = 0.0;
  double max_ratio = 0.0;
  double min_ratio = std::numeric_limits<double>::max();
  double max_t1 = args_info.maxt1_arg;
  int nb = 0;

  // Loop for patient
  for(auto p:patients) {
    // Patient infos
    table << p.synfrizz_id << p.name << p.injected_activity_in_MBq;
    // Compute activity at first timepoint
    double ia = p.injected_activity_in_MBq;
    std::vector<Timepoint> timepoints;
    sdb->GetTimepoints(p, timepoints);
    if (timepoints.size() < 1) {
      table << 0 << 0 << 0 << 0;
      LOG(WARNING) << "Error no timepoints for patient " << p;
      continue;
    }
    double t1 = timepoints[0].time_from_injection_in_hours;
    double l = Lambda_Indium_in_hours;
    double iat1 = ia * exp(-l * t1);

    // Get total nb of counts in this first timepoints spect image
    std::string filename = sdb->GetSpectImagePath(timepoints[0]);
    ImageType::Pointer spect = syd::ReadImage<ImageType>(filename);
    itk::StatisticsImageFilter<ImageType>::Pointer filter = itk::StatisticsImageFilter<ImageType>::New();
    filter->SetInput(spect);
    filter->Update();
    double sum = filter->GetSum();
    double ratio = sum/iat1;
    table << t1 << iat1 << sum << ratio;

    // Record some stat
    if (p.synfrizz_id != 0) {
      if (t1 < max_t1) {
        mean_ratio += sum/iat1;
        if (ratio > max_ratio) max_ratio = ratio;
        if (ratio < min_ratio) min_ratio = ratio;
        nb++;
      }
    }

  }
  table.Print(std::cout);

  mean_ratio /= (double)nb;
  std::cout << "#" << nb << " mean = " << mean_ratio
            << "\t inv = " << std::setprecision(25) << 1.0/mean_ratio
            << std::setprecision(0)
            << " \t min = " << min_ratio << " \t max = " << max_ratio << std::endl;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
