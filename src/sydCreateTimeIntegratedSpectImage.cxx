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
#include "sydCreateTimeIntegratedSpectImage_ggo.h"
#include "sydTimeIntegratedSpectImageFilter.h"

// easylogging : only once initialization (in the main)
INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

using namespace syd;

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydCreateTimeIntegratedSpectImage, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // User can provide images and times values
  typedef double PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  typedef itk::Image<float, 3> FloatImageType;
  std::vector<std::string> filenames;
  std::vector<double> times;
  std::vector<double> calibration_factors;
  double scalingFactor = 1.0;
  if (args_info.images_given > 0) {
    for(auto i=0; i<args_info.images_given; i++) filenames.push_back(args_info.images_arg[i]);
    if (args_info.times_given != args_info.images_given) LOG(FATAL) << "You need to provide as many 'times' than 'images'";
    for(auto i=0; i<args_info.times_given; i++) times.push_back(args_info.times_arg[i]);
    calibration_factors.resize(args_info.times_given);
    for(auto i=0; i<calibration_factors.size(); i++) calibration_factors[i] = 1.0;
  }
  else {
    // Check args
    if( args_info.inputs_num < 2) {
      LOG(FATAL) << "Error, provide 2 options : <databasename> <patientname>";
    }

    // Read DB : must be a adb
    std::string db_name = args_info.inputs[0];
    std::shared_ptr<ActivityDatabase> adb = Database::OpenDatabaseType<ActivityDatabase>(db_name);
    std::shared_ptr<StudyDatabase> sdb = adb->get_study_database();
    std::shared_ptr<ClinicDatabase> cdb = sdb->get_clinical_database();

    // Query
    std::string patient_name = args_info.inputs[1];
    Patient patient = cdb->GetPatientByName(patient_name);

    // Retrieve the list of n spects
    std::vector<Timepoint> timepoints;
    sdb->GetTimepoints(patient, timepoints);
    scalingFactor = 1000000.0; // in million to avoid rounding error during computation
    for(auto t:timepoints) {
      RawImage spect = sdb->GetById<RawImage>(t.spect_image_id);
      filenames.push_back(sdb->GetImagePath(spect));
      calibration_factors.push_back(1.0/t.calibration_factor/patient.injected_activity_in_MBq*scalingFactor);
    }

    // Get the list of times
    for(auto t:timepoints) times.push_back(t.time_from_injection_in_hours);
    std::string s;
    for(auto t:times) s = s+" "+syd::toString(t);
    ELOG(1) << "Times : " << s;

  }

  // Open images
  std::vector<ImageType::Pointer> spects;
  for(auto f:filenames) {
    ELOG(1) << "Reading " << f;
    ImageType::Pointer im  = syd::ReadImage<ImageType>(f);
    spects.push_back(im);
  }

  // Start algorithm
  syd::TimeIntegratedSpectImageFilter::Pointer filter = syd::TimeIntegratedSpectImageFilter::New();
  for(auto i=0; i<spects.size(); i++) filter->AddInput(times[i], spects[i], calibration_factors[i]);
  filter->SetMinimumActivityValue(args_info.min_arg*calibration_factors[0]);
  filter->SetDebugFlag(args_info.debug_flag);
  filter->SetDefaultPixelValue(0.0);
  filter->SetProgressBarFlag(!args_info.nopb_flag);
  filter->Initialise();
  filter->Update();
  ImageType::Pointer o = filter->GetOutput();

  // If the image was scaled, we "unscale"
  if (scalingFactor != 1.0) {
    ScaleImage<ImageType>(o, 1.0/scalingFactor);
  }

  // Store results (convert to float)
  FloatImageType::Pointer fo = syd::CastImageToFloat<ImageType>(o);
  syd::WriteImage<FloatImageType>(fo, args_info.output_arg);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
