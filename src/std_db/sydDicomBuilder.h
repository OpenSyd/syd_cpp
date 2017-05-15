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

#ifndef SYDDICOMBUILDER_H
#define SYDDICOMBUILDER_H

// syd
#include "sydStandardDatabase.h"
#include "sydDicomFile.h"
#include "sydDicomSerie.h"
#include "sydDicomStruct.h"
#include "sydDicomUtils.h"

// --------------------------------------------------------------------
namespace syd {

  /// Search for dicom file in folder and insert the
  /// DicomSerie and DicomFile in the db.  A check on the patient
  /// name/id is performed (could be forced with
  /// SetForcePatientFlag(true)).
  class DicomBuilder {

  public:
    /// Constructor.
    DicomBuilder(syd::StandardDatabase * db);

    /// Destructor (empty)
    virtual ~DicomBuilder();

    /// Create a DicomSerie/DicomFile (still not inserted into the db,
    /// use UpdateDicomSerie for that)
    void SearchDicomInFile(std::string filename,
                           syd::Patient::pointer patient,
                           bool update_patient_info_from_file_flag);

    /// Create a DicomStruct (still not inserted into the db,
    /// use UpdateDicomSerie for that)
    void SearchDicomStructInFile(std::string filename,
                                 syd::Patient::pointer patient,
                                 bool update_patient_info_from_file_flag);

    /// Insert the created DicomSerie/DicomFile/DicomStruct into the db
    void InsertDicom();

    /// Get the current DicomSeries to be inserted
    syd::DicomSerie::vector GetDicomSeries() const { return dicom_series_to_insert; }

    /// Get the current DicomStruct to be inserted
    syd::DicomStruct::vector GetDicomStruct() const { return dicom_struct_to_insert; }

    /// Update the information from the file
    void UpdateDicomSerie(syd::DicomSerie::pointer serie);

  protected:
    syd::StandardDatabase * db;

    syd::DicomSerie::vector dicom_series_to_insert;
    syd::DicomFile::vector  dicom_series_dicom_files_to_insert;
    syd::DicomSerie::vector dicom_files_corresponding_series;
    std::vector<std::string> dicom_series_filenames_to_copy;

    syd::DicomStruct::vector dicom_struct_to_insert;
    syd::DicomFile::vector  dicom_struct_dicom_files_to_insert;
    std::vector<std::string> dicom_struct_filenames_to_copy;

    int nb_of_skip_files;
    int nb_of_skip_copy;

    void UpdateDicomSerie(DicomSerie::pointer serie,
                          const std::string & filename,
                          itk::GDCMImageIO::Pointer dicomIO);

    DicomFile::pointer CreateDicomFile(const std::string & filename,
                                       itk::GDCMImageIO::Pointer dicomIO,
                                       DicomSerie::pointer serie);

    bool GuessDicomSerieForThisFile(const std::string & filename,
                                    itk::GDCMImageIO::Pointer dicomIO,
                                    DicomSerie::pointer & serie);

    /// Insert the created DicomSerie/DicomFile into the db
    int InsertDicomSeries();

    /// Insert the created DicomStruct/DicomFile into the db
    int InsertDicomStruct();

    syd::DicomFile::pointer FindDicomFile(const std::string & sop_uid);

    syd::DicomStruct::pointer GetOrCreateDicomStruct(const gdcm::DataSet & dataset, std::string filename);

    void SetDicomPatient(syd::DicomBase::pointer dicom,
                         syd::Patient::pointer patient,
                         bool update_patient_info_from_file_flag);

    void UpdateDicomStruct(syd::DicomStruct::pointer dicom_struct,
                           const gdcm::DataSet & dataset);

    void UpdateDicomStructPatient(syd::DicomStruct::pointer dicom_struct,
                                  const gdcm::DataSet & dataset);

    typedef itk::MetaDataDictionary DictionaryType;

  }; // class DicomBuilder
} // namespace syd
// --------------------------------------------------------------------

#endif
