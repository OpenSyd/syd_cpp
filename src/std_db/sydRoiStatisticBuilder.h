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

#ifndef SYDROISTATISTICBUILDER_H
#define SYDROISTATISTICBUILDER_H

// syd
#include "sydStandardDatabase.h"
#include "sydImage.h"

// itk
#include <itkImageBase.h>

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to create a Image. A Image is defined
  /// with an injection, a patient, some tag, some associated dicom
  /// series and images.
  class RoiStatisticBuilder {

  public:
    /// Constructor.
    RoiStatisticBuilder(syd::StandardDatabase * db);

    /// Destructor (empty)
    ~RoiStatisticBuilder() {}

    /// Set the input image (required)
    void SetImage(syd::Image::pointer im);

    /// Set the mask (not required)
    void SetRoiMaskImage(syd::RoiMaskImage::pointer m);

    /// Look for the RoiMaskImage associated with this roitype and the image
    void SetRoiType(std::string name);

    /// Create a RoiStatistic. It it not inserted into the db
    syd::RoiStatistic::pointer ComputeStatistic();

  protected:
    /// Protected constructor. No need to use directly.
    RoiStatisticBuilder();

    /// Set the pointer to the database
    void SetDatabase(syd::StandardDatabase * db) { db_ = db; }

    /// Pointer to the database
    syd::StandardDatabase * db_;

    /// Pointer to the input image
    syd::Image::pointer input;

    /// Pointer to the mask
    syd::RoiMaskImage::pointer mask;

    /// Pointer to the output image
    syd::Image::pointer output;

  }; // class RoiStatisticBuilder

} // namespace syd
// --------------------------------------------------------------------

#endif
