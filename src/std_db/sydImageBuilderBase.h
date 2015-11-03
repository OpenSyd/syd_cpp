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

#ifndef SYDIMAGEBUILDERBASE_H
#define SYDIMAGEBUILDERBASE_H

// syd
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// This base class is used to create ImageBuilder class that perform operation on Images
  /// series and images.
  class ImageBuilderBase {

  public:
    /// Constructor.
    ImageBuilderBase(syd::StandardDatabase * db) { SetDatabase(db); }

    /// Set the pointer to the database
    void SetDatabase(syd::StandardDatabase * db) { db_ = db; }


    syd::Image::pointer InsertNewMHDImageLike(syd::Image::pointer image);
    syd::Image::pointer InsertNewMHDImage(syd::Patient::pointer patient);
    syd::Image::pointer InsertNewMHDImage(syd::DicomSerie::pointer dicom);
    syd::RoiMaskImage::pointer InsertNewMHDRoiMaskImage(syd::Patient::pointer patient, syd::RoiType::pointer roitype);


    void InitializeMHDFiles(syd::Image::pointer image);

    void CheckMHDImage(syd::Image::pointer image);

    /// Update image information from the associated file (pixel type, size, md5 etc)
    void UpdateImageFromFile(syd::Image::pointer image, std::string filename);

    /// Update image information from an itk image, and write the image file in the db (type, size, spacing, md5)
    template<class PixelType>
    void UpdateImage(syd::Image::pointer image, typename itk::Image<PixelType,3>::Pointer & itk_image);

    /// Change the filename to the default one (mv file if already exist)
    void RenameToDefaultFilename(syd::Image::pointer image);
    void RenameToDefaultFilename(syd::RoiMaskImage::pointer mask);

    // // Create a new (empty) image and insert it in the db, using the
    // // default filename that uses the dicom modality
    // syd::Image::pointer InsertNewEmptyImage(syd::DicomSerie & dicom);

    // FIXME
    // TODO syd::FlipImageIfNegativeAxis<ImageType>(itk_image);
    // TODO UpdateMD5(syd::Image::pointer);


  protected:
    /// Protected constructor. No need to use directly.
    ImageBuilderBase();

    /// Pointer to the database
    syd::StandardDatabase * db_;

    // /// When creating a new image, create the associated file
    // void InsertFile(syd::Image::pointer image,
    //                 const std::string & filename,
    //                 const std::string & relativepath,
    //                 bool deleteExistingFiles=false);

    // /// When creating a new image, create the associated file
    // void InsertFile(syd::Image::pointer image,
    //                 const std::string & path,
    //                 bool deleteExistingFiles=false);

    // /// When creating a new image, create the associated files with
    // /// default name. The image *must* be persistant in the db, with an id.
    // void InsertFiles(syd::Image::pointer image);

  }; // class ImageBuilderBase

#include "sydImageBuilderBase.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
