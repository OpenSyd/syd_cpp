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

#ifndef SYDDECAYCORRECTEDIMAGEBUILDER_H
#define SYDDECAYCORRECTEDIMAGEBUILDER_H

// syd
#include "sydImageBuilderBase.h"
#include "sydCalibration.h"

// --------------------------------------------------------------------
namespace syd {

  /// Convert a spect image by correcting for the decay.
  /// Input pixel image should be in 'counts' unity
  /// Output pixel will be in MBq by injected MBq  and decay corrected.
  class DecayCorrectedImageBuilder: public syd::ImageBuilderBase {

  public:
    /// Constructor.
    DecayCorrectedImageBuilder(StandardDatabase * db):ImageBuilderBase(db) {}

    /// Main function
    Image::pointer InsertDecayCorrectedImage(Image::pointer image, Calibration::pointer calib);

  }; // class DecayCorrectedImageBuilder
} // namespace syd
// --------------------------------------------------------------------

#endif
