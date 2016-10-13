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

#ifndef SYDIROIMASKMAGEHELPER_H
#define SYDIROIMASKMAGEHELPER_H

// syd
#include "sydImageHelper.h"

// --------------------------------------------------------------------
namespace syd {


  // Search a roi type by name
  syd::RoiType::pointer FindRoiType(const std::string & roiname,
                                    syd::StandardDatabase * db);


  // Search a roi mask image by a roi name and the image
  syd::RoiMaskImage::pointer FindRoiMaskImage(const syd::Image::pointer image,
                                              const std::string & roi_name);

  /// Compute the default image path (based on the patient's name)
  std::string GetDefaultImageRelativePath(syd::RoiMaskImage::pointer mask);

  /// Compute the default image mhd filename (based on id + modality)
  std::string GetDefaultMhdImageFilename(syd::RoiMaskImage::pointer mask);

  /// Create a new roi mask image
  syd::RoiMaskImage::pointer InsertRoiMaskImageFromFile(std::string filename,
                                                        syd::Patient::pointer patient,
                                                        syd::RoiType::pointer roitype);

  /// Create a new roi mask image
  syd::RoiMaskImage::pointer InsertRoiMaskImageFromFile(std::string filename,
                                                        syd::Image::pointer image,
                                                        syd::RoiType::pointer roitype);

  /// Search for a single RoiStatistic with the given image+mask
  syd::RoiStatistic::pointer FindOneRoiStatistic(syd::Image::pointer image,
                                                 syd::RoiMaskImage::pointer mask);

  /// Search for a single RoiStatistic with the given tia + roiname
  syd::RoiStatistic::pointer FindOneRoiStatistic(syd::TiaImage::pointer tia,
                                                 std::string roi_name);

  // Mask could be null (compute stat on the whole image)
  syd::RoiStatistic::pointer InsertRoiStatistic(syd::Image::pointer image,
                                                syd::RoiMaskImage::pointer mask,
                                                std::string mask_output_filename="");

  /// Find the stat or create it if not exist. Return nullptr if no RoiMaskImage
  /// exist for the given roi_name
  syd::RoiStatistic::pointer InsertOrUpdate(syd::Image::pointer image,
                                            std::string roi_name,
                                            bool force_update);

  // Compute the statistic according to the stat's image and
  // mask. return the used mask image (that have been resampled).
  // if itk_mask2 is given, a 'AND' is performed between the stat mask and the itk_mask2.
  itk::Image<unsigned char, 3>::Pointer
    UpdateRoiStatistic(syd::RoiStatistic::pointer stat,
                       const itk::Image<unsigned char, 3>::Pointer itk_mask2=nullptr);


} // namespace syd
// --------------------------------------------------------------------

#endif
