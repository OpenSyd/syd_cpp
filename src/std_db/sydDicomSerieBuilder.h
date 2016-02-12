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
    ~DicomSerieBuilder() {}

    /// Set the pointer to the database
    void SetDatabase(StandardDatabase * db) { db_ = db; }

    /// Set the injection (required)
    void SetInjection(Injection::pointer injection);

    /// Don't stop even if the patient in the dicom seems different
    /// from the patient in the injection
    void SetForcePatientFlag(bool b) { forcePatientFlag_ = b; }

    /// If the file already exist in the db, we delete it and insert a new one
    void SetForceUpdateFlag(bool b) { forceUpdateFlag_ = b; }

    /// Create a DicomSerie/DicomFile (still not inserted into the db,
    /// use UpdateDicomSerie for that)
    void CreateDicomSerieFromFile(std::string filename);

    /// Insert the created DicomSerie/DicomFile into the db
    void InsertDicomSeries();

    /// Update the information from the file
    void UpdateDicomSerie(DicomSerie::pointer serie);

  protected:
    /// Protected constructor. No need to use directly.
    DicomSerieBuilder();

    StandardDatabase * db_;
    Patient::pointer patient_;
    Injection::pointer injection_;
    bool forcePatientFlag_;
    bool useInjectionFlag_;
    bool forceUpdateFlag_;

    DicomSerie::vector series_to_insert;
    DicomSerie::vector series_to_update;
    DicomFile::vector dicomfiles_to_insert;
    std::vector<std::string> files_to_copy;
    std::vector<std::string> destination_folders;
    int nb_of_skip_files;

    void UpdateDicomSerie(DicomSerie::pointer serie,
                          const std::string & filename,
                          DcmObject * dset);
    DicomFile::pointer CreateDicomFile(const std::string & filename,
                                       DcmObject * dset,
                                       DicomSerie::pointer serie);
    bool GuessDicomSerieForThisFile(const std::string & filename,
                                    DcmObject * dset,
                                    DicomSerie::pointer & serie);
    bool DicomFileAlreadyExist(const std::string & sop_uid);

  }; // class DicomSerieBuilder
} // namespace syd
// --------------------------------------------------------------------

#endif
