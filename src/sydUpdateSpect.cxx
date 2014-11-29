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
#include "sydUpdateSpect_ggo.h"
#include "core/sydCommon.h"
#include "sydClinicDatabase.h"
#include "sydStudyDatabase.h"

// easylogging : only once initialization (in the main)
_INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

using namespace syd;

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydUpdateSpect, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if( args_info.inputs_num < 2) {
    LOG(FATAL) << "Error, provide 2 options : <databasename> and <patientname>.";
  }

  // Read DB
  std::string db_name = args_info.inputs[0];
  std::shared_ptr<StudyDatabase> sdb =
    Database::OpenDatabaseType<StudyDatabase>(db_name);
  std::shared_ptr<ClinicDatabase> cdb = sdb->get_clinical_database();

  // Get patient name
  std::string patient_name = args_info.inputs[1];

  // Update SPECT images for the given patient
  Patient patient = cdb->GetPatientByName(patient_name);
  std::vector<Timepoint> timepoints;
  sdb->LoadVector<Timepoint>(odb::query<Timepoint>::patient_id == patient.id, timepoints);

  // Loop over all spect images
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  for (auto t:timepoints) {
    RawImage spect(sdb->GetById<RawImage>(t.spect_image_id));
    std::string filename = sdb->GetImagePath(spect);
    ImageType::Pointer spectimage = syd::ReadImage<ImageType>(filename);
    VLOG(1) << "Updating " << filename;

    // Multiply values
    double scale = 1.0/4.0;
    typedef itk::MultiplyImageFilter<ImageType, ImageType, ImageType> FilterType;
    typename FilterType::Pointer filter = FilterType::New();
    filter->SetInput(spectimage);
    filter->SetConstant(scale);
    filter->Update();

    // Save
    syd::WriteImage<ImageType>(filter->GetOutput(), filename);

    sdb->Update(spect);
  }


  // This is the end, my friend.
}
// --------------------------------------------------------------------
