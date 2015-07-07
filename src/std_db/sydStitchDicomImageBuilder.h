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

#ifndef SYDSTITCHDICOMIMAGEBUILDER_H
#define SYDSTITCHDICOMIMAGEBUILDER_H

// syd
#include "sydStandardDatabase.h"
#include "sydImage.h"

// itk
#include <itkImageBase.h>

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to stitch 2 dicoms together and create an image.
  class StitchDicomImageBuilder {

  public:
    /// Constructor.
    StitchDicomImageBuilder(syd::StandardDatabase * db);

    /// Destructor (empty)
    ~StitchDicomImageBuilder() {}

    /// Create & Insert a new Image by stitching 2 dicoms
    syd::Image::pointer InsertStitchedImage(const syd::DicomSerie::pointer a, const syd::DicomSerie::pointer b);

  protected:
    /// Protected constructor. No need to use directly.
    StitchDicomImageBuilder();

    /// Set the pointer to the database
    void SetDatabase(syd::StandardDatabase * db) { db_ = db; }

    /// Pointer to the database
    syd::StandardDatabase * db_;

  }; // class StitchDicomImageBuilder

  //#include "sydStitchDicomImageBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
