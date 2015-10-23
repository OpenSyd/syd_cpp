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

#ifndef SYDROISTATISTIC_H
#define SYDROISTATISTIC_H

// syd
#include "sydRoiMaskImage.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::RoiStatistic")
  /// Simple table to store a label and a description
  class RoiStatistic : public syd::Record {
  public:

    /// linked image
    syd::Image::pointer image;

    /// linked mask
    syd::RoiMaskImage::pointer mask;

    /// Stat values
    double mean;
    double std_dev;
    double n;
    double min;
    double max;
    double sum;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(RoiStatistic);
    TABLE_DECLARE_MANDATORY_FUNCTIONS(RoiStatistic);
    TABLE_DECLARE_OPTIONAL_FUNCTIONS(RoiStatistic);
    // ------------------------------------------------------------------------

  protected:
    RoiStatistic();

  }; // end of class

}
// --------------------------------------------------------------------

#endif
