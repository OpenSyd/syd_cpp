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

    /// Get the mask of the given image that is of roiname and has the
    /// same frame_of_reference_uid. Raise exception is not found
    syd::RoiMaskImage::pointer FindMask(const syd::Image::pointer input,
                                        const std::string roiname);

    /// Retrieve a RoiStatistic with the  given image and mask, if exists.
    bool Exists(syd::RoiStatistic::pointer * stat,
                const syd::Image::pointer image,
                const syd::RoiMaskImage::pointer mask);

    /// Create a RoiStatistic. It it not inserted into the db
    void ComputeStatistic(syd::RoiStatistic::pointer stat);

    /// Set the empty pixel value
    void SetEmptyPixelValue(double  e) { emptyPixelValue_ = e; }

    /// Consider or not an empty pixel value
    void SetEmptyPixelValueFlag(bool b) { useEmptyPixelValueFlag_ = b; }

  protected:
    /// Protected constructor. No need to use directly.
    RoiStatisticBuilder();

    /// Set the pointer to the database
    void SetDatabase(syd::StandardDatabase * db) { db_ = db; }

    /// Pointer to the database
    syd::StandardDatabase * db_;

    // Use an empty pixel value (not count in the computation)
    bool useEmptyPixelValueFlag_;

    // Empty pixel value (not count in the computation)
    double emptyPixelValue_;

  }; // class RoiStatisticBuilder

} // namespace syd
// --------------------------------------------------------------------

#endif
