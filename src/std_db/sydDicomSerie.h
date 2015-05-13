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
#include <array>

// syd
#include "sydPatient.h"
#include "sydInjection.h"
#include "sydFile.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object
  /// Store information about a dicom image (serie). Element of table
  /// 'DicomSerie' stored in a db. Contains information about a dicom
  /// image.
  class DicomSerie : public syd::TableElement {
  public:

#pragma db id auto
    /// Id of the DicomSerie
    IdType id;

#pragma db not_null
    /// Foreign key, it must exist in the Patient table. Not strictly
    /// needed but convenient.
    std::shared_ptr<syd::Patient> patient;

#pragma db not_null
    /// Foreign key, it must exist in the Injection table.
    std::shared_ptr<syd::Injection> injection;

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

    /// Image size (in pixels)
    std::array<int, 3> size;

    /// Image spacing aka size of the pixel (in mm)
    std::array<double, 3> spacing; //FIXME

    // ------------------------------------------------------------------------
    SET_TABLE_NAME("DicomSerie")
    DicomSerie();
    ~DicomSerie();
    DicomSerie(const DicomSerie & other);
    DicomSerie & operator= (const DicomSerie & other);
    void copy(const DicomSerie & t);

    virtual std::string ToString() const;
    virtual void SetValues(std::vector<std::string> & arg);

  }; // end class
}
// --------------------------------------------------------------------

#endif
