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
  class RoiStatistic : public syd::Record,
                       public syd::RecordWithHistory,
                       public syd::RecordWithTags {
  public:

#pragma db not_null on_delete(cascade)
    /// Linked image. If the image is deleted, the RoiStatistic also.
    syd::Image::pointer image;

#pragma db on_delete(cascade)
    /// Linked mask. May be null (no mask). The RoiStatistic is deleted if the mask is.
    syd::RoiMaskImage::pointer mask;

    /// Stat values
    double mean;
    double std_dev;
    double n;
    double min;
    double max;
    double sum;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(RoiStatistic, syd::RoiStatistic);
    // ------------------------------------------------------------------------

    /// Write the element as a string
    virtual std::string ToString() const;

    virtual void DumpInTable(syd::PrintTable2 & table) const;

  protected:
    RoiStatistic();

  }; // end of class

} // end namespace
// --------------------------------------------------------------------

#endif
