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

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::Image")
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

    /// List of associated files.
    syd::File::vector files;

    /// Dicoms that serve to compute this image (could be empty).
    syd::DicomSerie::vector dicoms;

    /// Type of the image (mhd by default)
    std::string type;

    /// Pixel type: short, float, unsigned char
    std::string pixel_type;

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

    void UpdateFile(syd::Database * db, const std::string & filename,
                    const std::string & relativepath, bool deleteExistingFiles=false);

  protected:
    Image();

  }; // end class
// --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
