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

#ifndef SYDROISTATISTICHELPER_H
#define SYDROISTATISTICHELPER_H

// syd
#include "sydRoiStatistic.h"
#include "sydTiaImage.h"
#include "sydImageUtils.h"

// --------------------------------------------------------------------
namespace syd {


  /// Search all RoiStatistic with the given image+mask. mask maybe nullptr.
  syd::RoiStatistic::vector FindRoiStatistic(syd::Image::pointer image,
                                             syd::RoiMaskImage::pointer mask);

  // Mask could be null (compute stat on the whole image)
  syd::RoiStatistic::pointer NewRoiStatistic(syd::Image::pointer image,
                                             syd::RoiMaskImage::pointer mask,
                                             std::string mask_output_filename="");

  // Mask could be null (compute stat on the whole image)
  syd::RoiStatistic::pointer NewRoiStatistic(syd::Image::pointer image,
                                             syd::RoiMaskImage::pointer mask,
                                             syd::Image::pointer mask2,
                                             std::string mask_output_filename="");

  // Mask could be null (compute stat on the whole image)
  syd::RoiStatistic::pointer NewRoiStatistic(syd::TiaImage::pointer tia,
                                             syd::RoiMaskImage::pointer mask,
                                             std::string mask_output_filename="");

  // Compute the statistic according to the stat's image and mask. return the
  // used mask image (that have been resampled). if itk_mask2 is given, a 'AND'
  // is performed between the stat mask and the itk_mask2.
  itk::Image<unsigned char, 3>::Pointer
    UpdateRoiStatistic(syd::RoiStatistic::pointer stat,
                       const itk::Image<unsigned char, 3>::Pointer itk_mask2=nullptr);


} // namespace syd
// --------------------------------------------------------------------

#endif
