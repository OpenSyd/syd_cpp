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

// syd
#include "sydPatient.h"
#include "sydDicomFile.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::DicomSerie") callback(Callback)
  /// Store basic information about a dicom image (serie).
  class DicomSerie : public syd::Record {
  public:

    virtual ~DicomSerie() { }

#pragma db not_null
    /// Foreign key, it must exist in the Patient table.
    syd::Patient::pointer patient;

    /// List of DicomFile
    syd::DicomFile::vector dicom_files;

    /// Date when the image has been acquired. Dicom tag =
    /// AcquisitionTime & AcquisitionDate
    std::string dicom_acquisition_date;

    /// Date when the image has been reconstructed. Dicom tag =
    /// ContentDate/Time or InstanceCreationDate/Tiem
    std::string dicom_reconstruction_date;

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

    /// Concatenation of several descriptions tag (SeriesDescription
    /// StudyDescription, ImageID, DatasetName, Manufacturer)
    std::string dicom_description;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(DicomSerie, syd::DicomSerie);
    // ------------------------------------------------------------------------

    /// Write the element as a string
    virtual std::string ToString() const;

    virtual std::string ComputeRelativeFolder() const;

    void Callback(odb::callback_event, odb::database&) const;
    void Callback(odb::callback_event, odb::database&);

    virtual void DumpInTable(syd::PrintTable & table) const;
    virtual void DumpInTable_default(syd::PrintTable & table) const;
    virtual void DumpInTable_file(syd::PrintTable & table) const;
    virtual void DumpInTable_filelist(syd::PrintTable & table) const;
    virtual void DumpInTable_details(syd::PrintTable & table) const;

    /// Check if the associated files exist on disk
    virtual syd::CheckResult Check() const;

  protected:
    DicomSerie();

  }; // end class
}
// --------------------------------------------------------------------

#endif
