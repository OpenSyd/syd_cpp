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

#ifndef SYDROITIMEPOINTS_H
#define SYDROITIMEPOINTS_H

// syd
#include "sydTimepoints.h"
#include "sydRoiStatistic.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::RoiTimepoints") callback(Callback)
  /// Store information about a timepoints build from some RoiStatistic
  class RoiTimepoints: public syd::Timepoints {
  public:

    DEFINE_TABLE_CLASS(RoiTimepoints);

    /// list of RoiStatistic. (note: on_delete cascade does not work here because vector)
#pragma db not_null on_delete(cascade)
    syd::RoiStatistic::vector roi_statistics;

    /// Write the element as a string
    virtual std::string ToString() const;
    virtual std::string AllFieldsToString() const {return ToString();}

    /// Build a string to compute MD5
    virtual std::string ToStringForMD5() const;

  protected:
    RoiTimepoints();

  }; // end class
  // ------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
