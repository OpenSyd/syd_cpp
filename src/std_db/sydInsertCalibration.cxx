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

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertCalibration, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // Get the tag
  std::string tagname = args_info.inputs[0];
  syd::Tag::vector tags;
  db->FindTags(tags, tagname);
  DDS(tags);

  // Get the image
  syd::IdType image_id = atoi(args_info.inputs[1]);
  syd::Image::pointer image;
  db->QueryOne(image, image_id);
  DD(image);

  // Check
  if (image->dicoms.size() == 0) {
    LOG(FATAL) << "Error, the image is not associated with a dicom. ";
  }
  syd::DicomSerie::pointer dicom = image->dicoms[0];
  if (dicom->injection == NULL) {
    LOG(FATAL) << "Error, the dicom is not associated with an injection.";
  }
  syd::Injection::pointer injection = dicom->injection;
  DD(injection);

  // Already exist or not ?
  syd::Calibration::vector calibrations;
  syd::Calibration::pointer calibration;
  int n=0;
  typedef odb::query<syd::Calibration> CQ;
  CQ q = CQ::image == image->id;
  db->Query(calibrations, q);
  for(auto c:calibrations) {
    if (syd::IsAllTagsIn(c->tags, tags)) {
      if (n==1) {
        LOG(FATAL) << "Error, at least two calibrations match the image and the tags list."
                   << std::endl << image
                   << std::endl << syd::GetLabels(tags);
      }
      ++n;
      calibration = c;
    }
  }
  if (n==0) { //not found, we create
    DD("Create new calibration");
    calibration = syd::Calibration::New();
    calibration->image = image;
    for(auto t:tags) calibration->tags.push_back(t);
    db->Insert(calibration);
  }
  DD(calibration);


  // Now we update the values
  double f = atof(args_info.inputs[2])/100.0;
  if (f < 0.0 or f > 100.0) {
    LOG(FATAL) << "Error the fov_ratio must be [0:100] %";
  }
  calibration->fov_ratio = f;
  DD(calibration->fov_ratio);

  double injected_activity = injection->activity_in_MBq;
  double time = syd::DateDifferenceInHours(dicom->acquisition_date, injection->date);
  double lambda = log(2.0)/(injection->radionuclide->half_life_in_hours);
  DD(injected_activity);
  DD(time);
  DD(lambda);
  double activity_at_acquisition = injected_activity * exp(-lambda * time);
  DD(activity_at_acquisition);

  // Compute the image total counts
  DD(db->GetAbsolutePath(image));
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  ImageType::Pointer itk_image = syd::ReadImage<ImageType>(db->GetAbsolutePath(image));
  itk::ImageRegionConstIterator<ImageType> iter(itk_image, itk_image->GetLargestPossibleRegion());
  double total_counts = 0;
  while (!iter.IsAtEnd()) {
    total_counts += iter.Get();
    ++iter;
  }
  DD(total_counts);

  // Compute the calibration and update
  double k = total_counts / activity_at_acquisition * f;
  DD(k);
  calibration->factor = k;
  db->Update(calibration);
  DD(calibration);

  // This is the end, my friend.
}
// --------------------------------------------------------------------
