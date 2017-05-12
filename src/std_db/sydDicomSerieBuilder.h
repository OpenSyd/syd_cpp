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
#include "sydDicomUtils.h"

// --------------------------------------------------------------------
namespace syd {

  /// Search for dicom file in folder and insert the
  /// DicomSerie and DicomFile in the db.  A check on the patient
  /// name/id is performed (could be forced with
  /// SetForcePatientFlag(true)).
  class DicomSerieBuilder {

  public:
    /// Constructor.
    DicomSerieBuilder(syd::StandardDatabase * db);

    /// Destructor (empty)
    virtual ~DicomSerieBuilder();

    /// Create a DicomSerie/DicomFile (still not inserted into the db,
    /// use UpdateDicomSerie for that)
    void SearchDicomInFile(std::string filename,
                           syd::Patient::pointer patient,
                           bool update_patient_info_from_file_flag);

    /// Insert the created DicomSerie/DicomFile into the db
    syd::DicomSerie::vector InsertDicomSeries();

    /// Update the information from the file
    void UpdateDicomSerie(syd::DicomSerie::pointer serie);

  protected:
    syd::StandardDatabase * db;
    syd::DicomSerie::vector series_to_insert;
    syd::DicomFile::vector dicomfiles_to_insert;
    std::vector<std::string> files_to_copy;
    int nb_of_skip_files;

    void UpdateDicomSerie(DicomSerie::pointer serie,
                          const std::string & filename,
                          itk::GDCMImageIO::Pointer dicomIO);

    DicomFile::pointer CreateDicomFile(const std::string & filename,
                                       itk::GDCMImageIO::Pointer dicomIO,
                                       DicomSerie::pointer serie);

    bool GuessDicomSerieForThisFile(const std::string & filename,
                                    itk::GDCMImageIO::Pointer dicomIO,
                                    DicomSerie::pointer & serie);

    bool DicomFileAlreadyExist(const std::string & sop_uid);

    typedef itk::MetaDataDictionary DictionaryType;

  }; // class DicomSerieBuilder
} // namespace syd
// --------------------------------------------------------------------

#endif
