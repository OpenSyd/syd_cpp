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
#include "sydDatabase.h"
#include "Patient-odb.hxx"
#include "Serie-odb.hxx"
#include "RoiType-odb.hxx"
#include "sydDicomCommon.h"

// This database manage the following set of tables : Patient, Serie, RoiTypes
// --------------------------------------------------------------------
namespace syd {

  class ClinicDatabase: public Database {

  public:
    ClinicDatabase(std::string name, std::string param);
    ~ClinicDatabase() {}

    SYD_INIT_DATABASE(ClinicDatabase);

    // Query the DB
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

    // Functions for Dump & Print
    virtual void Dump(std::ostream & os, std::vector<std::string> & args);
    std::string Print(Patient patient, int level=0);
    std::string Print(Serie serie);

  protected:
    int check_file_content_level_;

  }; // class ClinicDatabase
} // namespace syd
// --------------------------------------------------------------------

#endif
