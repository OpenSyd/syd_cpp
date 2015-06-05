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

    /// The resulting images will be associated with this tag
    void SetImageTag(syd::Tag & tag) { tag_ = tag; }

    /// Create & Insert a new Image from this DicomSerie
    syd::Image InsertImageFromDicomSerie(const syd::DicomSerie & dicomserie);

    /// Create & Insert new images for all dicom in this
    /// timepoint. Warning to not check if some images are already
    /// associated with this timepoint.
    void InsertImagesFromTimepoint(syd::Timepoint & timepoint);

    /// Update image information from this itk image (type, size, spacing)
    template<class PixelType>
    void UpdateImageInfo(syd::Image & image, typename itk::Image<PixelType,3>::Pointer & itk_image, bool computeMD5Flag);

    /// Update and insert the Files associated with an mhd image
    void InsertImageMHDFilesInfo(syd::Image & image, std::string filename);

    /// Create & Insert a new Image by stitching 2 dicoms
    syd::Image StitchDicomSerie(const syd::DicomSerie & a, const syd::DicomSerie & b);

    /// Propose a default filename for the image (use the image.id, so must be inserted in the db before)

    std::string GetDefaultImageFilename(const syd::Image & image);
    std::string GetDefaultImageRelativePath(const syd::Image & image);
    std::string GetDefaultRoiMaskImageFilename(const syd::RoiMaskImage & mask);
    std::string GetDefaultRoiMaskImageRelativePath(const syd::RoiMaskImage & mask);

    syd::Image CreateNewMHDImageFromDicom(const syd::Tag & tag,
                                          const syd::DicomSerie & dicomserie);

    void ImageSetDicom(syd::Image & image, const syd::DicomSerie & dicomserie);
    void ImageInsertFile(syd::Image & image, std::string relativepath, std::string filename);


    syd::RoiMaskImage InsertRoiMaskImageFromDicomSerie(const syd::DicomSerie & dicom,
                                                       const syd::RoiType & roitype,
                                                       const std::string & filename);


    // FIXME
    template<class PixelType>
    typename itk::Image<PixelType,3>::Pointer ReadImage(const syd::DicomSerie & dicom);

  protected:
    /// Protected constructor. No need to use directly.
    ImageBuilder();

    /// Set the pointer to the database
    void SetDatabase(syd::StandardDatabase * db) { db_ = db; }

    syd::StandardDatabase * db_;
    syd::Tag tag_;

  }; // class ImageBuilder

#include "sydImageBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
