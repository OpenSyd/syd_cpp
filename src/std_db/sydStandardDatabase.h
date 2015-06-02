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

#ifndef SYDSTANDARDDATABASE_H
#define SYDSTANDARDDATABASE_H

// syd
#include "sydPrintTable.h"
#include "sydDatabase.h"
#include "sydTable.h"
#include "sydDatabaseManager.h"

// syd tables
#include "sydPatient-odb.hxx"
#include "sydRadionuclide-odb.hxx"
#include "sydInjection-odb.hxx"
#include "sydDicomSerie-odb.hxx"
#include "sydDicomFile-odb.hxx"
#include "sydFile-odb.hxx"
#include "sydTag-odb.hxx"
#include "sydTimepoint-odb.hxx"
#include "sydImage-odb.hxx"

// --------------------------------------------------------------------
namespace syd {

  /// This database manages a set of 'standard' tables: Patient, Injection etc.
  class StandardDatabase: public Database {

  public:

    /// Generic element insertion.
    virtual TableElementBase * InsertFromArg(const std::string & table_name, std::vector<std::string> & arg);

    /// Insert a new Injection (check patient before etc)
    syd::Injection * InsertInjection(std::vector<std::string> & arg);

    /// Return the patient by name or study_id. Fail if not found.
    syd::Patient FindPatientByNameOrStudyId(const std::string & arg);

    /// Return the injection for the given patient where radionuclide match the arg. Fail if not found
    syd::Injection FindInjectionByNameOrId(const Patient & patient,
                                           const std::string & arg);

    /// Return dicom series that match the patterns
    void FindDicom(const syd::Patient & patient,
                   const std::vector<std::string> & patterns,
                   std::vector<syd::DicomSerie> & series);

    /// Return the folder where the dicomserie are stored
    std::string GetAbsoluteFolder(const DicomSerie & serie);

    /// Return the folder where the patient images are stored
    std::string GetAbsoluteFolder(const Patient & patient);

    /// Return the folder where the patient images are stored (relative to the database folder)
    std::string GetRelativeFolder(const Patient & patient);

    /// Return the absolute path of the file
    std::string GetAbsolutePath(const File & file);

    // /// Simple default Dump overwritten here to select special case DumpDicom.
    // virtual void Dump(const std::vector<std::string> & args, std::ostream & os);

    /// Insert a new Timepoint
    syd::Timepoint InsertTimepoint(syd::Injection & injection,
                                   syd::Tag & tag,
                                   std::vector<syd::DicomSerie> & dicoms,
                                   bool replaceTimepointFlag);

    /// Specific Dump for Timepoint
    virtual void DumpTimepoint(const std::vector<std::string> & args, std::ostream & os, bool verboseFlag=false);

    /// Delete a list of elements for the given table
    //    virtual void Delete(const std::string & table_name, std::vector<syd::IdType> & ids);

    // template<class TableElement>
    // void Delete(TableElementBase & e) { DD("standard delete db"); GetTable<TableElement>()->Delete(e); }


    //    virtual void OnDelete(const std::string & table_name, IdType id); //TableElementBase & e);
    virtual void OnDelete(const std::string & table_name, TableElementBase * e);
    void OnDeleteImage(syd::Image & e);
    void OnDeleteFile(syd::File & e);

  protected:
    /// Insert the tables
    virtual void CreateTables();

  }; // class StandardDatabase

} // namespace syd

// --------------------------------------------------------------------

#endif
