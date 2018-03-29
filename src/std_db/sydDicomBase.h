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

#ifndef SYDDICOMBASE_H
#define SYDDICOMBASE_H

// syd
#include "sydInjection.h"
#include "sydDicomFile.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object abstract pointer(std::shared_ptr)
  /// Store basic information about a dicom (both for DicomSerie or DicomStruct)
  /// Abstract class -> this is not a table in the db, but it is shared by other
  /// tables (DicomSerie and DicomStruct). Must not be defined ''callback',
  /// because will call two times.
  class DicomBase:
    public syd::RecordWithComments,
    public syd::RecordWithTags {
  public:

      /// Define pointer type
      typedef std::shared_ptr<DicomBase> pointer;

      /// Define vectortype
      typedef std::vector<pointer> vector;

#pragma db not_null
      /// Foreign key, it must exist in the Patient table.
      syd::Patient::pointer patient;

      /// List of DicomFile
      syd::DicomFile::vector dicom_files;

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

      /// Dicom tags
      std::string dicom_series_description;
      std::string dicom_study_description;
      std::string dicom_study_name;
      std::string dicom_study_id;
      std::string dicom_image_id;
      std::string dicom_dataset_name;
      std::string dicom_manufacturer;
      std::string dicom_manufacturer_model_name;
      std::string dicom_software_version;
      std::string dicom_patient_name;
      std::string dicom_patient_id;
      std::string dicom_patient_birth_date;
      std::string dicom_patient_sex;

      /// Write the element as a string
      virtual std::string ToString() const;
      virtual std::string AllFieldsToString() const;

      /// Compute the folder to store the dicom
      virtual std::string ComputeRelativeFolder() const = 0;

      /// Database callback (MUST be called in derived class)
      void Callback(odb::callback_event, odb::database&) const;

      /// Database callback (MUST be called in derived class)
      void Callback(odb::callback_event, odb::database&);

      /// Check if the associated files exist on disk. MUST be redefined in
      /// derived class because syd::Record also defined it.
      virtual syd::CheckResult Check() const;

      /// To easy 'BuildFields' in DicomSerie + DicomStruct
      static std::function<std::string(pointer)> GetDicomFileFunction();

  protected:
      DicomBase();

    }; // end class
}
// --------------------------------------------------------------------
#endif
