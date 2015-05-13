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

#ifndef SYDCLINICDATABASE_H
#define SYDCLINICDATABASE_H

// syd
#include "sydPrintTable.h"
#include "sydDatabase.h"
#include "sydTable.h"
#include "sydPatient-odb.hxx"
#include "sydInjection-odb.hxx"
#include "sydDicomSerie-odb.hxx"
#include "sydDicomFile-odb.hxx"
#include "sydFile-odb.hxx"
//#include "RoiType-odb.hxx"
//#include "sydDicomCommon.h"
#include "sydDatabaseManager.h"

// --------------------------------------------------------------------
namespace syd {

  /// This database manages three tables : Patient, Injection
  class ClinicDatabase: public Database {

  public:

    /// Generic element insertion.
    virtual TableElement * InsertFromArg(const std::string & table_name, std::vector<std::string> & arg);

    /// Insert a new Injection (check patient before etc)
    syd::Injection * InsertInjection(std::vector<std::string> & arg);

    /// Return the patient by name or study_id. Stop if not found.
    bool QueryPatientByNameOrStudyId(const std::string & arg,
                                     Patient & p, bool fatalIfNotFound=true);

    /// Return all the patients by name (can be 'all') or study_id. Stop if not found.
    void QueryPatientsByNameOrStudyId(const std::string & arg,
                                      std::vector<Patient> & patients);

    /// Return the injection for the given patient where radionuclide match the arg.
    bool QueryInjectionByNameOrId(const Patient & patient,
                                  const std::string & arg,
                                  Injection & p, bool fatalIfNotFound=true);

    /// Return the folder where the dicomserie are stored
    std::string GetAbsoluteFolder(const DicomSerie & serie);

    /// Return the folder where the patient images are stored
    std::string GetAbsoluteFolder(const Patient & patient);

    // /// Simple default Dump overwritten here to select special case DumpDicom.
    // virtual void Dump(const std::vector<std::string> & args, std::ostream & os);

    /// Specific Dump for Dicom
    void DumpDicom(std::ostream & os,
                   syd::Patient & patient,
                   const std::vector<std::string> & patterns,
                   double max_time_diff);



    // ------------- OLD below


    /// Dump information about patients
    void DumpPatients(const std::vector<std::string> & args, std::ostream & os);

    /// Dump information about series
    void DumpSeries(const std::vector<std::string> & args, std::ostream & os);

    /*! Search for all patients with name matching the \p pattern. It can
      be empty or \c 'all', in that case return all patients. It can
      be a list of patient names, eventually with the special
      character \c '%' to match any set of letters. */
    void QueryPatientsByName(const std::string & pattern, std::vector<Patient> & patients);

    /// Return the patient \p name. Stop if not found.
    Patient QueryPatientByName(const std::string & name);


    // FIXME trial for dicom/serie
    //    syd::DicomSerie & CreateDicomSerie(const IdType patient_id, const std::vector<std::string> & filename);

    /*
    std::string GetSeriePath(IdType serie_id);
    std::string GetPatientPath(IdType patient_id);
    std::string GetPath(const Patient & patient);
    std::string GetPath(const Serie & serie);
    void GetPatientsByName(const std::string & patient_name, std::vector<Patient> & patients);
    Patient GetPatientByName(const std::string & patient_name);
    RoiType GetRoiType(const std::string & name);
    Patient GetPatient(const Serie & serie);
    odb::query<Serie> GetSeriesQueryFromPatterns(std::vector<std::string> patterns);
    void AndSeriesQueryFromPattern(odb::query<Serie> & q, std::string pattern);
    void GetAssociatedCTSerie(IdType serie_id,
                              std::vector<std::string> & patterns,
                              Serie & serie);
    */

    /*
    // Update and create new element
    void UpdateSerie(Serie & serie);

    // Function for checking integrity
    void set_check_file_content_level(int l) { check_file_content_level_ = l; }
    virtual void CheckIntegrity(std::vector<std::string> & args);
    void CheckIntegrity(const Patient & patient);
    void CheckPatient(const Patient & patient);
    void CheckSerie(const Serie & serie);
    void CheckSerie_CT(const Serie & serie);
    void CheckSerie_NM(const Serie & serie);
    void CheckFile(OFString filename);
    */

    //std::string Print(Patient patient, int level=0);
    //std::string Print(Serie serie);

  protected:
    /// Insert the tables
    virtual void CreateTables();

  }; // class ClinicDatabase

} // namespace syd

// --------------------------------------------------------------------

#endif
