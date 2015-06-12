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

#pragma db object
  /// Store information about a dicom image (serie). Element of table
  /// 'Image' stored in a db. Contains information about a dicom
  /// image.
  class Image : public syd::TableElementBase {
  public:

#pragma db id auto
    /// Id of the Image
    IdType id;

#pragma db not_null
    /// Foreign key, it must exist in the Patient table.
    std::shared_ptr<syd::Patient> patient;

    /// Associated Tag. FIXME : or a vector ? Delete image or not ???
    std::vector<std::shared_ptr<syd::Tag>> tags;

    /// List of associated files.
    std::vector<std::shared_ptr<syd::File>> files;

    // FIXME -> needed ?. dicoms that serve to compute this image (could
    // be empty).
    std::vector<std::shared_ptr<syd::DicomSerie>> dicoms;

    std::string type; // mhd etc. Needed ?

    std::string pixel_type; // float, uint, Needed ?

    unsigned short int dimension; // Needed ?

    /// Image size (in pixels)
    std::array<int, 3> size; // Needed ?

    /// Image spacing aka size of the pixel (in mm)
    std::array<double, 3> spacing; // Needed

    // ------------------------------------------------------------------------
    SET_TABLE_NAME("Image")
    Image();

    virtual std::string ToString() const;

    bool operator==(const Image & p);
    bool operator!=(const Image & p) { return !(*this == p); }

    virtual void OnDelete(syd::Database * db);

    std::string GetAcquisitionDate() const;
    std::string GetModality() const;

    /// Add a tag to the list (check is already exist), but do not update in the db.
    void AddTag(syd::Tag & tag);

  }; // end class
// --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
