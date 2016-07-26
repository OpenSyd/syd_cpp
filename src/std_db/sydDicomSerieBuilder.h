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

#ifndef SYDDICOMSERIEBUILDER_H
#define SYDDICOMSERIEBUILDER_H

// syd
#include "sydStandardDatabase.h"
#include "sydDicomFile.h"
#include "sydDicomSerie.h"

// --------------------------------------------------------------------
namespace syd {

  /// Search for dicom file in folder and insert the
  /// DicomSerie and DicomFile in the db.  A check on the patient
  /// name/id is performed (could be forced with
  /// SetForcePatientFlag(true)).
  class DicomSerieBuilder {

  public:
    /// Constructor.
    DicomSerieBuilder(StandardDatabase * db);

    /// Destructor (empty)
    virtual ~DicomSerieBuilder();

    /// Set the pointer to the database
    void SetDatabase(StandardDatabase * db) { db_ = db; }

    /// Set the patient (required)
    void SetPatient(Patient::pointer p);

    /// Don't stop even if the patient in the dicom seems different
    /// from the patient
    void SetForcePatientFlag(bool b) { forcePatientFlag_ = b; }

    /// Create a DicomSerie/DicomFile (still not inserted into the db,
    /// use UpdateDicomSerie for that)
    void SearchDicomInFile(std::string filename);

    /// Insert the created DicomSerie/DicomFile into the db
    void InsertDicomSeries();

    /// Update the information from the file
    void UpdateDicomSerie(DicomSerie::pointer serie);

  protected:
    /// Protected constructor. No need to use directly.
    DicomSerieBuilder();

    StandardDatabase * db_;
    Patient::pointer patient_;
    bool forcePatientFlag_;

    DicomSerie::vector series_to_insert;
    DicomSerie::vector series_to_update;
    DicomFile::vector dicomfiles_to_insert;
    std::vector<std::string> files_to_copy;
    std::vector<std::string> destination_folders;
    int nb_of_skip_files;

    void UpdateDicomSerie(DicomSerie::pointer serie,
                          const std::string & filename,
                          itk::GDCMImageIO::Pointer dicomIO);

    DicomFile::pointer CreateDicomFile(const std::string & filename,
                                       itk::GDCMImageIO::Pointer dicomIO,
                                       DicomSerie::pointer serie);
    bool GuessDicomSerieForThisFile(const std::string & filename,
                                    itk::GDCMImageIO::Pointer dicomIO,
                                    //DcmObject * dset,
                                    DicomSerie::pointer & serie);
    bool DicomFileAlreadyExist(const std::string & sop_uid);

    typedef itk::MetaDataDictionary DictionaryType;

    std::string GetStringValueFromTag(itk::GDCMImageIO::Pointer dicomIO,
                                      const std::string & key);
    double GetDoubleValueFromTag(itk::GDCMImageIO::Pointer dicomIO,
                                 const std::string & key);
    unsigned short GetUShortValueFromTag(itk::GDCMImageIO::Pointer dicomIO,
                                         const std::string & key);

  }; // class DicomSerieBuilder
} // namespace syd
// --------------------------------------------------------------------

#endif
