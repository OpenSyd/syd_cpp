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

#ifndef SYDIMAGEBUILDER_H
#define SYDIMAGEBUILDER_H

// syd
#include "sydStandardDatabase.h"

// itk
#include <itkImageBase.h>

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to create a Image. A Image is defined
  /// with an injection, a patient, some tag, some associated dicom
  /// series and images.
  class ImageBuilder {

  public:
    /// Constructor.
    ImageBuilder(syd::StandardDatabase * db);

    /// Destructor (empty)
    ~ImageBuilder() {}

    void SetTag(syd::Tag & tag) { tag_ = tag; }
    void SetPatient(syd::Patient & patient) { patient_ = patient; }

    /// TODO
    syd::Image CreateImageFromDicomSerie(const syd::DicomSerie & dicomserie);

    template<class ImageType>
    void UpdateImageInfo(syd::Image & image, typename ImageType::Pointer & itk_image);

  protected:
    /// Protected constructor. No need to use directly.
    ImageBuilder();

    /// Set the pointer to the database
    void SetDatabase(syd::StandardDatabase * db) { db_ = db; }

    syd::StandardDatabase * db_;
    syd::Tag tag_;
    syd::Patient patient_;

  }; // class ImageBuilder

#include "sydImageBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
