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
#include "sydRoiMaskImageHelper.h"
#include "sydRoiStatisticHelper.h"
#include "sydTagHelper.h"
#include "sydImageCrop.h"
#include "sydImageAnd.h"
#include "sydFAFMask.h"

// itk
#include <itkLabelStatisticsImageFilter.h>

// --------------------------------------------------------------------
syd::RoiType::pointer syd::FindRoiType(const std::string & roiname,
                                       syd::StandardDatabase * db)
{
  try {
    syd::RoiType::pointer r;
    odb::query<syd::RoiType> q = odb::query<RoiType>::name == roiname;
    db->QueryOne(r, q);
    return r;
  } catch (const std::exception& e) {
    EXCEPTION("Cannot find a RoiType named '" << roiname << "'.");
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::vector
syd::FindRoiMaskImage(const syd::Image::pointer image,
                      const std::string & roi_name)
{
  auto db = image->GetDatabase<syd::StandardDatabase>();
  syd::RoiType::pointer roitype = syd::FindRoiType(roi_name, db); // will exception if not found
  syd::RoiMaskImage::vector rois;
  odb::query<syd::RoiMaskImage> q =
    odb::query<syd::RoiMaskImage>::patient == image->patient->id and
    odb::query<syd::RoiMaskImage>::roitype == roitype->id and
    odb::query<syd::RoiMaskImage>::frame_of_reference_uid == image->frame_of_reference_uid;
  db->Query(rois, q);
  return rois;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer
syd::FindOneRoiMaskImage(const syd::Image::pointer image,
                         const std::string & roi_name)
{
  auto masks = FindRoiMaskImage(image, roi_name);
  if (masks.size() == 1) return masks[0];
  if (masks.size() == 0) {
    EXCEPTION("Cannot find a RoiMaskImage for image " << image->id
              << " named '" << roi_name << "' (with same frame_of_reference_uid).");
  }
  EXCEPTION("Several RoiMaskImage exist for image " << image->id
              << " named '" << roi_name << "' (with same frame_of_reference_uid).");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer
syd::InsertRoiMaskImageFromFile(std::string filename,
                                syd::Patient::pointer patient,
                                syd::RoiType::pointer roitype)
{
  auto db = patient->GetDatabase<syd::StandardDatabase>();
  auto mask = db->New<syd::RoiMaskImage>();
  mask->patient = patient;
  mask->type = "mhd";
  mask->roitype = roitype;
  mask->modality = "mask"; // force to mask
  mask->pixel_unit = syd::FindPixelUnit(db, "label");
  db->Insert(mask);
  mask->files = syd::InsertFilesFromMhd(db, filename,
                                        mask->ComputeDefaultRelativePath(),
                                        mask->ComputeDefaultMHDFilename());
  syd::SetImageInfoFromFile(mask);
  db->Update(mask);
  return mask;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::ComputeMass(syd::Image::pointer ct, std::string roi_name)
{
  // Check
  if (ct->modality != "CT" or ct->pixel_unit->name != "HU") {
    EXCEPTION("Cannot estimate the mass from this image " << ct);
  }

  // find roimask
  auto mask = syd::FindOneRoiMaskImage(ct, roi_name);
  if (!mask) {
    EXCEPTION("In ComputeScalingFactorToMBperKg, cannot find a mask '"
              << roi_name << "' for the image " << ct);
  }

  // find roistatistic or compute it
  auto stat = syd::FindOneRoiStatistic(ct, mask);
  if (!stat) {
    stat = syd::NewRoiStatistic(ct, mask);
    auto db = ct->GetDatabase<syd::StandardDatabase>();
    db->Insert(stat);
    LOG(1) << "Create RoiStatistic " << stat;
  }

  // Get the mean HU, convert to mean density in g/cm3
  auto mean_hu = stat->mean;
  double mu_water = 1.0;
  double mu_air = 0.0;
  auto density = mean_hu/1000.0*(mu_water-mu_air)+mu_water;

  // Get the volume
  double voxel_vol_mm3 = ct->spacing[0]*ct->spacing[1]*ct->spacing[2];
  double vol_cc = voxel_vol_mm3*stat->n*0.001;

  // Compute the mass in kg
  double mass = vol_cc * density / 1000.0;
  return mass;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<typename MaskType>
syd::RoiMaskImage::pointer
syd::InsertRoiMask(typename MaskType::Pointer itk_mask,
                   syd::Patient::pointer patient,
                   syd::RoiType::pointer roitype)
{
  // save the image
  auto db = patient->GetDatabase();
  std::string filename = db->GetUniqueTempFilename();
  syd::WriteImage<MaskType>(itk_mask, filename);

  // insert the files
  auto image = syd::InsertRoiMaskImageFromFile(filename, patient, roitype);

  // Update size and spacing
  syd::DeleteMHDImage(filename);
  syd::SetImageInfoFromFile(image);
  db->Update(image);
  return image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer
syd::InsertFAFMask(const syd::Image::pointer input_SPECT,
                   const syd::Image::pointer input_planar)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType3D;
  typedef itk::Image<PixelType, 2> ImageType2D;
  auto itk_input_SPECT = syd::ReadImage<ImageType3D>(input_SPECT->GetAbsolutePath());
  auto itk_input_planar = syd::ReadImage<ImageType2D>(input_planar->GetAbsolutePath());
  auto fafMask = syd::ComputeFAFMask<ImageType2D, ImageType3D>(itk_input_SPECT, itk_input_planar);

  // Create the syd image
  auto db = input_planar->patient->GetDatabase<syd::StandardDatabase>();
  auto roiType = syd::FindRoiType("FAF", db);
  return syd::InsertRoiMask<ImageType2D>(fafMask, input_planar->patient, roiType);
}
// --------------------------------------------------------------------
