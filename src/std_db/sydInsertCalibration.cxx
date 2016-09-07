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
#include "sydInsertCalibration_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertCalibration, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the fow ratio
  double f = atof(args_info.inputs[0])/100.0;
  if (f < 0.0 or f > 100.0) {
    LOG(FATAL) << "Error the fov_ratio must be [0:100] %";
  }

  // Get the image
  std::vector<syd::IdType> ids;
  syd::ReadIdsFromInputPipe(ids);
  for(auto i=1; i<args_info.inputs_num; i++) {
    ids.push_back(atoi(args_info.inputs[i]));
  }
  syd::Image::vector images;
  db->Query(images, ids);
  if (images.size() == 0) {
    LOG(FATAL) << "No image ids given. I do nothing.";
  }

  // Loop on images
  for(auto image:images) {
    // Check
    if (image->dicoms.size() == 0) {
      LOG(FATAL) << "Error, the image is not associated with a dicom. ";
    }
    syd::DicomSerie::pointer dicom = image->dicoms[0];
    syd::Injection::pointer injection = image->injection;

    // Already exist or not ?
    std::string status;
    syd::Calibration::vector calibrations;
    syd::Calibration::pointer calibration;
    typedef odb::query<syd::Calibration> CQ;
    CQ q = CQ::image == image->id;
    db->Query(calibrations, q);
    int n = calibrations.size();
    if (n >= 1) {
      syd::Tag::vector tags = calibrations[0]->tags;
      int m=0;
      for(auto c:calibrations) {
        if (syd::IsAllTagsIn(c->tags, tags)) {
          if (m == 1) {
            LOG(FATAL) << "Error, at least two calibrations match the image and the tags list."
                       << std::endl << image
                       << std::endl << syd::GetLabels(tags);
          }
          ++m;
          calibration = c;
        }
      }
    }
    if (n==0) { //not found, we create
      status = "(new)";
      calibration = syd::Calibration::New();
      calibration->image = image;
      db->Insert(calibration);
    }
    else status = "(updated)";

    // Now we update the values
    calibration->fov_ratio = f;

    double injected_activity = injection->activity_in_MBq;
    double time = syd::DateDifferenceInHours(dicom->dicom_acquisition_date, injection->date);
    double lambda = log(2.0)/(injection->radionuclide->half_life_in_hours);
    double activity_at_acquisition = injected_activity * exp(-lambda * time);

    // Compute the image total counts
    typedef float PixelType;
    typedef itk::Image<PixelType,3> ImageType;
    ImageType::Pointer itk_image = syd::ReadImage<ImageType>(db->GetAbsolutePath(image));
    itk::ImageRegionConstIterator<ImageType> iter(itk_image, itk_image->GetLargestPossibleRegion());
    double total_counts = 0;
    while (!iter.IsAtEnd()) {
      total_counts += iter.Get();
      ++iter;
    }

    // Compute the calibration and update
    double k = total_counts / activity_at_acquisition * f;
    calibration->factor = k;
    db->UpdateTagsFromCommandLine(calibration->tags, args_info);
    db->Update(calibration);
    LOG(1) << calibration << " " << status;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
