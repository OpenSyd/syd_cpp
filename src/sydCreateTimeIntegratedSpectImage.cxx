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
#include "sydCommon.h"
#include "sydClinicDatabase.h"
#include "sydStudyDatabase.h"
#include "sydTimeIntegratedSpectImageFilter.h"

// easylogging : only once initialization (in the main)
_INITIALIZE_EASYLOGGINGPP

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

  // Variables needed to for final units conversion
  double calibration_factor = 1.0;
  double injected_activity_in_MBq = 1.0;

  // User can provide images and times values
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  typedef itk::Image<float, 3> FloatImageType;
  std::vector<std::string> filenames;
  std::vector<double> times;
  if (args_info.images_given > 0) {
    for(auto i=0; i<args_info.images_given; i++) filenames.push_back(args_info.images_arg[i]);
    if (args_info.times_given != args_info.images_given) LOG(FATAL) << "You need to provide as many 'times' than 'images'";
    for(auto i=0; i<args_info.times_given; i++) times.push_back(args_info.times_arg[i]);
    DDS(filenames);
    DDS(times);
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
    DD(patient_name);
    Patient patient = cdb->GetPatientByName(patient_name);
    DD(patient);

    // Retrieve the list of n spects
    std::vector<Timepoint> timepoints;
    sdb->GetTimepoints(patient, timepoints);
    for(auto t:timepoints) {
      RawImage spect = sdb->GetById<RawImage>(t.spect_image_id);
      filenames.push_back(sdb->GetImagePath(spect));
    }

    // Get the list of times
    for(auto t:timepoints) times.push_back(t.time_from_injection_in_hours);
    DDS(times);

    // Get values for units conversion
    calibration_factor = adb->Get_CountByMM3_in_MBqByCC(1.0);
    injected_activity_in_MBq = patient.injected_activity_in_MBq;
  }

  // Open images
  std::vector<ImageType::Pointer> spects;
  for(auto f:filenames) {
    VLOG(1) << "Reading " << f;
    ImageType::Pointer im  = syd::ReadImage<ImageType>(f);
    spects.push_back(im);
  }

  // Start algorithm
  /*ImageType::Pointer tii =
    syd::CreateTimeIntegratedSpectImage<ImageType>(spects, times);
  */
  syd::TimeIntegratedSpectImageFilter::Pointer filter = syd::TimeIntegratedSpectImageFilter::New();
  for(auto i=0; i<spects.size(); i++) filter->AddInput(times[i], spects[i]);
  filter->SetMinimumActivityValue(args_info.min_arg);
  filter->Initialise();
  filter->Update();
  ImageType::Pointer o = filter->GetOutput();

  // Conversion from MBq.h/cc in MBq by injected
  if (args_info.images_given == 0) {
    DD(calibration_factor);
    DD(injected_activity_in_MBq);
    double vol = o->GetSpacing()[0]*o->GetSpacing()[1]*o->GetSpacing()[2]/1000.0; // in cc
    DD(vol);
    itk::ImageRegionIterator<ImageType> iter(o, o->GetLargestPossibleRegion());
    while (!iter.IsAtEnd()) {
      double v = iter.Get();
      v = v*calibration_factor/1000.0/injected_activity_in_MBq; // unit is MBq.h so in millions of counts by MBq injected
      //v = v*calibration_factor/injected_activity_in_MBq; // unit is kBq.h so in thousands of counts by MBq injected
      iter.Set(v);
      ++iter;
    }
  }
  FloatImageType::Pointer fo = syd::CastImageToFloat<ImageType>(o);

  // Store results (convert from float to double)
  syd::WriteImage<FloatImageType>(fo, "bidon.mhd"); // FIXME

  // This is the end, my friend.
}
// --------------------------------------------------------------------
