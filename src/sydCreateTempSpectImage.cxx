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
#include "sydCreateTempSpectImage_ggo.h"
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
  GGO(sydCreateTempSpectImage, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Check args
  if( args_info.inputs_num < 3) {
    LOG(FATAL) << "Error, provide 3 options : <databasename> <patientname> <timepoint>";
  }

  // Read DB : must be a sdb
  std::string db_name = args_info.inputs[0];
  std::shared_ptr<ActivityDatabase> adb =
    Database::OpenDatabaseType<ActivityDatabase>(db_name);
  std::shared_ptr<StudyDatabase> sdb = adb->get_study_database();
  std::shared_ptr<ClinicDatabase> cdb = adb->get_clinical_database();

  // Get patient name
  std::string patient_name = args_info.inputs[1];

  // Get the timepoint number
  int number = atoi(args_info.inputs[2]);
  DD(number);

  // Update SPECT images for the given patient
  std::vector<Patient> patients;
  cdb->GetPatientsByName(patient_name, patients);
  DDS(patients);

  // loop patient
  for(auto patient:patients) {
    //std::vector<Timepoint> timepoints;
    //sdb->LoadVector<Timepoint>(odb::query<Timepoint>::patient_id == patient.id, timepoints);

    // Get injected activity
    double ia = patient.injected_activity_in_MBq;

    Timepoint timepoint;
    bool b =
      sdb->GetIfExist<Timepoint>(odb::query<Timepoint>::patient_id == patient.id and
                                 odb::query<Timepoint>::number == number, timepoint);
    if (!b) {
      LOG(FATAL) << "Could not find timepoint number " << number;
    }

    // open spect images
    typedef float PixelType;
    typedef itk::Image<PixelType, 3> ImageType;
    RawImage spect(sdb->GetById<RawImage>(timepoint.spect_image_id));
    std::string filename = sdb->GetImagePath(spect);
    ImageType::Pointer spectimage = syd::ReadImage<ImageType>(filename);
    ELOG(1) << "Loading " << filename;

    // open ct image
    typedef short CTPixelType;
    typedef itk::Image<CTPixelType, 3> CTImageType;
    RawImage ct(sdb->GetById<RawImage>(timepoint.ct_image_id));
    std::string filename_ct = sdb->GetImagePath(ct);
    CTImageType::Pointer ctimage = syd::ReadImage<CTImageType>(filename_ct);
    ELOG(1) << "Loading " << filename_ct;

    // resample like spect
    ctimage = syd::ResampleAndCropImageLike<CTImageType>(ctimage, spectimage, 1, -1000);
    syd::WriteImage<CTImageType>(ctimage, "bidon_ct.mhd");

    // Get voxel volume in mm3
    double vol = ctimage->GetSpacing()[0]*ctimage->GetSpacing()[1]*ctimage->GetSpacing()[2];
    DD(vol);

    itk::ImageRegionIterator<ImageType> iter(spectimage,spectimage->GetLargestPossibleRegion());
    itk::ImageRegionIterator<CTImageType> iter_ct(ctimage,ctimage->GetLargestPossibleRegion());
    while(!iter.IsAtEnd()) {
      double v = iter.Get();

      if (v>0.0) {
        // Get density of the pixel
        double density = (iter_ct.Get()/1000.0+1.0);
        // DD(iter_ct.Get());
        //DD(density);

        // Get activity and convert into MBqByCC
        v = v/vol; // count by mm3
        v = adb->Get_CountByMM3_in_MBqByCC(v); // convert into MBqByCC
        v = (v*density*1000.0); // Convert into MBqByKG
        v = v/ia*100.0;// Convert into PercentInjectedActivityByKG
        //DD(v);
        iter.Set(v);
      }

      // Next pixel
      ++iter;
      ++iter_ct;
    }

    // Save
    syd::replace(filename, ".mhd", "-pcidg.mhd");
    DD(filename);
    syd::WriteImage<ImageType>(spectimage, filename);
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
