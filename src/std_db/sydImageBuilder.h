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
#include "sydTableImage.h"

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

    // -----------------------------------------------------------
    /// Create & Insert a new Image from this DicomSerie
    syd::Image InsertImage(const syd::DicomSerie & dicomserie);

    /// Create & Insert a new Image by stitching 2 dicoms
    syd::Image InsertStitchedImage(const syd::DicomSerie & a, const syd::DicomSerie & b);

    /// Create & Insert a RoiMaskImage from an image.mhd
    syd::RoiMaskImage InsertRoiMaskImage(const syd::DicomSerie & dicom,
                                         const syd::RoiType & roitype,
                                         const std::string & filename);
    // -----------------------------------------------------------


    // -----------------------------------------------------------
    /// Update the image by creating the Files
    void UpdateFile(syd::Image & image, std::string relativepath, std::string filename);

    /// Update the image with dicom (just add the dicom to the list)
    void UpdateDicom(syd::Image & image, const syd::DicomSerie & dicomserie);

    /// Update image information from this itk image (type, size, spacing)
    template<class PixelType>
    void UpdateImageInfo(syd::Image & image,
                         typename itk::Image<PixelType,3>::Pointer & itk_image,
                         bool computeMD5Flag);

    /// Update an image by cropping according to the given mask
    void CropImageLike(syd::Image & image, syd::Image & mask, bool forceFlag=false);

    /// Crop and image like another (pixeltype known)
    template<class PixelType>
    void CropImageLike(syd::Image & image, syd::Image & like);

    /// Crop an image according to a threshold
    void CropImageWithThreshold(syd::Image & image, double threshold);

    /// Crop an image according to a threshold (pixeltype known)
    template<class PixelType>
    void CropImageWithThreshold(syd::Image & image, double lower_threshold);
    // -----------------------------------------------------------


    // -----------------------------------------------------------
    /// Propose a default filename for the image (use the image.id, so must be inserted in the db before)
    std::string GetDefaultImageFilename(const syd::Image & image);

    /// Propose a default path for the image
    std::string GetDefaultImageRelativePath(const syd::Image & image);

    /// Propose a default filename for the roimaskimage (use the image.id, so must be inserted in the db before)
    std::string GetDefaultRoiMaskImageFilename(const syd::RoiMaskImage & mask);

    /// Propose a default path for the roimaskimage (need the patient in image)
    std::string GetDefaultRoiMaskImageRelativePath(const syd::RoiMaskImage & mask);

    // Read an itk image for this dicom
    template<class PixelType>
    typename itk::Image<PixelType,3>::Pointer ReadImage(const syd::DicomSerie & dicom);


    /// OLD

    /// TODO. Create & Insert new images for all dicom in this
    /// timepoint. Warning to not check if some images are already
    /// associated with this timepoint.
    void InsertImagesFromTimepoint(syd::Timepoint & timepoint);


  protected:
    /// Protected constructor. No need to use directly.
    ImageBuilder();

    /// Set the pointer to the database
    void SetDatabase(syd::StandardDatabase * db) { db_ = db; }

    /// Pointer to the database
    syd::StandardDatabase * db_;

    /// Create & Insert a new image without information
    syd::Image InsertEmptyImage(const syd::Patient & patient);

  }; // class ImageBuilder

#include "sydImageBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
