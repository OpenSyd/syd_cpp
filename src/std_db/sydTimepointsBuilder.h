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

#ifndef SYDTIMEPOINTSBUILDER_H
#define SYDTIMEPOINTSBUILDER_H

// syd
#include "sydStandardDatabase.h"
#include "sydDatabaseFilter.h"

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to create a Timepoints (a set of
  /// times/values), from image and a mask, using the mean of the
  /// pixels inside the roi.
  class TimepointsBuilder: public syd::DatabaseFilter {

  public:
    /// Constructor.
    TimepointsBuilder(syd::StandardDatabase * db);

    /// Destructor (empty)
    ~TimepointsBuilder() {}

    void SetImages(const syd::Image::vector images);
    void SetRoiMaskImage(const syd::RoiMaskImage::pointer mask);

    /// Compute a Timepoints with mean pixels values in a roi images
    syd::Timepoints::pointer ComputeTimepoints();

  protected:
    syd::Image::vector images;
    syd::RoiMaskImage::pointer mask;

  }; // class TimepointsBuilder

} // namespace syd
// --------------------------------------------------------------------

#endif
