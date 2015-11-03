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

#ifndef SYDIMAGE_H
#define SYDIMAGE_H

// std
#include <array>

// syd
#include "sydPatient.h"
#include "sydInjection.h"
#include "sydFile.h"
#include "sydTag.h"
#include "sydDicomSerie.h"
#include "sydPixelValueUnit.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::Image") callback(Callback)
  /// Store information about a dicom image (serie). Element of table
  /// 'Image' stored in a db. Contains information about a dicom
  /// image.
  class Image : public syd::Record {
  public:

#pragma db not_null
    /// Foreign key, it must exist in the Patient table.
    syd::Patient::pointer patient;

    /// Associated tags
    syd::Tag::vector tags;

    /// List of associated files (will be deleted when the image is deleted)
    syd::File::vector files;

    /// Dicoms that serve to compute this image (could be empty).
    syd::DicomSerie::vector dicoms;

    /// Type of the image (mhd by default)
    std::string type;

    /// Pixel type: short, float, unsigned char
    std::string pixel_type;

    /// The unit of the pixel value (counts, suv, etc)
    syd::PixelValueUnit::pointer pixel_value_unit;

    /// Frame of reference : images with same frame_of_reference_uid
    /// are in the same coordinate system.
    std::string frame_of_reference_uid;

    /// Dimension is 3 or 2.
    unsigned short int dimension;

    /// Image size (in pixels)
    std::array<int, 3> size;

    /// Image spacing aka size of the pixel (in mm)
    std::array<double, 3> spacing;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(Image);
    TABLE_DECLARE_MANDATORY_FUNCTIONS(Image);
    TABLE_DECLARE_OPTIONAL_FUNCTIONS(Image);
    // ------------------------------------------------------------------------

    /// Standard folder
    virtual std::string ComputeRelativeFolder() const;

    /// Return the acquisition date of the first dicom linked to this image
    std::string GetAcquisitionDate() const;

    /// Return the modality of the dicom linked to this image
    std::string GetModality() const;

    /// Add a tag to the list (check is already exist) ; do not update in the db.
    void AddTag(syd::Tag::pointer tag);

    /// Remove a tag from the list ; do not update in the db. Do nothing it not found
    void RemoveTag(syd::Tag::pointer tag);

    /// Copy tags from another image
    void CopyTags(syd::Image::pointer tag);

    /// Add a DicomSerie to the list (check is already exist) ; do not update in the db.
    void AddDicomSerie(syd::DicomSerie::pointer dicom);

    /// Remove a DicomSerie from the list ; do not update in the db. Do nothing it not found
    void RemoveDicomSerie(syd::DicomSerie::pointer dicom);

    /// Copy DicomSeries from another image
    void CopyDicomSeries(syd::Image::pointer image);

    /// Callback : delete the associated files when the image is deleted.
    void Callback(odb::callback_event, odb::database&) const;
    void Callback(odb::callback_event, odb::database&);

    /// Test if the image is the same size and spacing than the param
    bool IsSameSizeAndSpacingThan(const syd::Image::pointer image) const;

    /// Return size as string such like 128x128x64
    std::string SizeAsString() const { return syd::ArrayToString<int,3>(size); }

    /// Return spacing as string such like 0.5x0.5x0.3
    std::string SpacingAsString() const { return syd::ArrayToString<double,3>(spacing); }

    /// Check if the image has a dicom. If not fail
    void FatalIfNoDicom() const;

  protected:
    Image();

  }; // end class
// --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
