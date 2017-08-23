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

#ifndef SYDDICOMSERIEHELPER_H
#define SYDDICOMSERIEHELPER_H

// syd
#include "sydDicomSerie.h"
#include "sydImageUtils.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  // Write the dicom into a mhd file, according to the given
  // pixel_type. If pixel_type is 'auto', the header of the dicom is
  // read to guess the pixel_type.
  void WriteDicomToMhd(syd::DicomSerie::pointer dicom,
                       std::string pixel_type,
                       std::string mhd_filename);

  // Write the dicom in the DicomSerie to a mhd file
  template<class ImageType>
    void WriteDicomToMhd(syd::DicomSerie::pointer dicom,
                         std::string mhd_filename);

  // Read a dicom into an itk_image
  template<class ImageType>
    typename ImageType::Pointer
    ReadDicomSerieImage(syd::DicomSerie::pointer dicom);

  /// Helper function (will change)
  template<typename F>
    F GetFctByPixelType(std::map<std::string, F> & map,
                        std::string pixel_type);

  /// Anonymize dicom serie
  void CopyDictionary (itk::MetaDataDictionary &fromDict, itk::MetaDataDictionary &toDict);
  syd::DicomSerie::pointer InsertAnonymizedDicomSerie(const syd::DicomSerie::pointer dicom);

  /// Return the date of the older dicom in the list
  std::string GetDateOfOlderDicom(const syd::DicomSerie::vector & dicoms);

  /// Try to guess if the 2 dicoms can be stitched
  bool IsDicomStitchable(const syd::DicomSerie::pointer a,
                         const syd::DicomSerie::pointer b,
                         double max_reconstruction_delay=1.0);

  /// Group dicoms by stitchable dicom
  std::vector<syd::DicomSerie::vector> GroupByStitchableDicom(syd::DicomSerie::vector dicoms);



  /// Set the dicom patient (display warning is different dicomID)
  void CheckAndSetPatient(syd::DicomBase::pointer dicom,
                          syd::Patient::pointer patient);

  /// Try to Find an existing patient from the dicom tag patient_dicom_id
  syd::Patient::pointer FindPatientFromDicomInfo(syd::StandardDatabase * db, syd::DicomBase::pointer dicom);

  /// Create a new patient according to patient_dicom_ids
  syd::Patient::pointer NewPatientFromDicomInfo(syd::StandardDatabase * db, syd::DicomBase::pointer dicom);

  /// Set patient info from the dicom (name, id, sex)
  void SetPatientInfoFromDicom(const syd::DicomBase::pointer dicom, syd::Patient::pointer patient);


  /// Create if needed the folder that will store the dicom
  void CreateDicomFolder(const syd::StandardDatabase * db, const syd::DicomBase::pointer dicom);

  /// Copy the filename according to dicom_file path
  bool CopyFileToDicomFile(const std::string & filename,
                           const syd::DicomFile::pointer dicom_file,
                           int log_level=3,
                           bool ignore_if_exist=true);

  /// Create file/path of DicomFile according to serie
  void SetDicomFilePathAndFilename(syd::DicomFile::pointer file,
                                   const std::string & filename,
                                   const syd::DicomSerie::pointer & serie);

  /// Create file/path of DicomFile according to struct
  void SetDicomFilePathAndFilename(syd::DicomFile::pointer file,
                                   const std::string & filename,
                                   const syd::DicomStruct::pointer & dicom_struct);

  /// Retrieve all Dicom for this patient
  syd::DicomSerie::vector FindDicomSeries(const syd::Patient::pointer patient);

}
#include "sydDicomSerieHelper.txx"
// --------------------------------------------------------------------

#endif
