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

#ifndef SYDDICOMSERIE_H
#define SYDDICOMSERIE_H

// std
#include <array> // needed on osx

// syd
#include "sydPatient.h"
#include "sydInjection.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::DicomSerie") callback(Callback)
  /// Store information about a dicom image (serie). Element of table
  /// 'DicomSerie' stored in a db. Contains information about a dicom
  /// image.
  class DicomSerie : public syd::Record {
  public:

#pragma db not_null
    /// Foreign key, it must exist in the Patient table. Useful if no associated injection
    syd::Patient::pointer patient;

    /// Foreign key. May be null. Dont delete Serie if injection does not exist.
    syd::Injection::pointer injection;

    /// Date when the image has been acquired. Dicom tag =
    /// AcquisitionTime & AcquisitionDate
    std::string acquisition_date;

    /// Date when the image has been reconstructed. Dicom tag =
    /// ContentDate/Time or InstanceCreationDate/Tiem
    std::string reconstruction_date;

    /// Dicom StudyInstanceUID
    std::string dicom_study_uid;

    /// Dicom SeriesInstanceUID. In general, slices of a same CT share
    /// the same SeriesInstanceUID. Sometimes SPECT acquisition at
    /// different table position share the same SeriesInstanceUID.
    std::string dicom_series_uid;

    /// Dicom FrameOfReferenceUID. to allow link between CT and SPECT
    /// image. "All images in a Series that share the same Frame of
    /// Reference UID shall be spatially related to each
    /// other.". Warning, sometimes CT and SPECT dot not share the
    /// same FrameOfReferenceUID even if they were acquired in the
    /// same session because, for example, of different table height.
    std::string dicom_frame_of_reference_uid;

    /// Modality as indicated in the dicom tag Modality
    std::string dicom_modality;

    /// Dicom tags Manufacturer and ManufacturerModelName
    std::string dicom_manufacturer;

    /// Concatenation of several descriptions tag (SeriesDescription
    /// StudyDescription, ImageID, DatasetName)
    std::string dicom_description;

    /// Image size (in pixels). It is an array because the size is fixed.
    std::array<int, 3> size;

    /// Image spacing aka size of the pixel (in mm)
    std::array<double, 3> spacing;

     // ------------------------------------------------------------------------
    TABLE_DEFINE(DicomSerie);
    TABLE_DECLARE_MANDATORY_FUNCTIONS(DicomSerie);
    // ------------------------------------------------------------------------

    virtual void Sort(DicomSerie::vector & v, const std::string & order);
    virtual void InitPrintTable(const syd::Database * db,
                                syd::PrintTable & ta,
                                const std::string & format) const;
    virtual void DumpInTable(const syd::Database * db,
                             syd::PrintTable & ta,
                             const std::string & format) const;
    virtual std::string ComputeRelativeFolder() const;
    void Callback(odb::callback_event, odb::database&) const;
    void Callback(odb::callback_event, odb::database&);

  }; // end class
}
// --------------------------------------------------------------------

#endif
