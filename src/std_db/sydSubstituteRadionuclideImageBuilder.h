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

#ifndef SYDSUBSTITUTERADIONUCLIDEIMAGEBUILDER_h
#define SYDSUBSTITUTERADIONUCLIDEIMAGEBUILDER_h

// syd
#include "sydStandardDatabase.h"
#include "sydImage.h"
#include "sydCalibration.h"

// --------------------------------------------------------------------
namespace syd {

  /// Convert a spect image by correcting for the decay.
  /// Input pixel image should be in 'counts' unity
  /// Output pixel will be in MBq by injected MBq  and decay corrected.
  class SubstituteRadionuclideImageBuilder {

  public:
    /// Constructor.
    SubstituteRadionuclideImageBuilder(StandardDatabase * db);

    /// Destructor (empty)
    ~SubstituteRadionuclideImageBuilder() {}

    /// Set the pointer to the database
    void SetDatabase(StandardDatabase * db) { db_ = db; }

    /// Main function
    Image::pointer CreateRadionuclideSubstitutedImage(Image::pointer image, Radionuclide::pointer rad);

  protected:
    /// Protected constructor. No need to use directly.
    SubstituteRadionuclideImageBuilder();

    StandardDatabase * db_;

  }; // class SubstituteRadionuclideImageBuilder
} // namespace syd
// --------------------------------------------------------------------

#endif
