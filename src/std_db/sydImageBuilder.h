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
#include "sydDatabaseFilter.h"

// --------------------------------------------------------------------
namespace syd {

  /// This base class is used to create ImageBuilder class that perform operation on Images
  /// series and images.
  class ImageBuilder: public syd::DatabaseFilter {

  public:
    /// Constructor.
    ImageBuilder(syd::StandardDatabase * db):DatabaseFilter(db) { }

    /// Create an empty image (also create associated file). Not inserted in the db.
    syd::Image::pointer NewMHDImage(syd::Patient::pointer patient);
    syd::Image::pointer NewMHDImage(syd::Injection::pointer injection);

    /// Create a new image and copy fields. Not inserted in the db.
    syd::Image::pointer NewMHDImageLike(syd::Image::pointer image);

    /// Create an image, using the dicom (patient). Not inserted in the db.
    syd::Image::pointer NewMHDImage(syd::DicomSerie::pointer dicom);

    /// Create and insert a new RoiMaskImage. Not inserted in the db.
    syd::RoiMaskImage::pointer NewMHDRoiMaskImage(syd::Patient::pointer patient,
                                                  syd::RoiType::pointer roitype);

    /// Copy the file image to an Image, updating all information
    /// (pixel type, size, md5 etc). DB not updated
    void CopyImageFromFile(syd::Image::pointer image, std::string filename);

    /// Set the image content from an itk image, and write it to the
    /// image file in the db (type, size, spacing, md5). DB not
    /// updated
    template<class PixelType>
    void SetImage(syd::Image::pointer image,
                  typename itk::Image<PixelType,3>::Pointer & itk_image);

    /// Change the filename to the default one (mv file if already exist). DB not updated.
    void RenameToDefaultFilename(syd::Image::pointer image);

    /// Change the filename to the default one (mv file if already exist). DB not updated.
    void RenameToDefaultFilename(syd::RoiMaskImage::pointer mask);

    /// helper: Insert, renametodefaultfilename and update
    void InsertAndRename(syd::Image::pointer image);

    /// helper: Insert, renametodefaultfilename and update
    void InsertAndRename(syd::RoiMaskImage::pointer mask);

    /// Search and set the pixel value unit
    void SetImagePixelValueUnit(syd::Image::pointer image, std::string pixel_unit);

  protected:
    /// Create the associated Files
    void InitializeMHDFiles(syd::Image::pointer image);

    /// Raise exception if the image could not be a mhd (2 Files are present etc)
    void CheckMHDImage(syd::Image::pointer image);

  }; // class ImageBuilder

#include "sydImageBuilder.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
