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
#include "sydImageUtils.h"

// syd tables
#include "sydPatient-odb.hxx"
#include "sydRadionuclide-odb.hxx"
#include "sydInjection-odb.hxx"
#include "sydDicomSerie-odb.hxx"
#include "sydDicomFile-odb.hxx"
#include "sydFile-odb.hxx"
#include "sydTag-odb.hxx"
#include "sydTimepoint-odb.hxx"
#include "sydRoiType-odb.hxx"
#include "sydRoiMaskImage-odb.hxx"
#include "sydImageTransform-odb.hxx"

// syd tables
#include "sydTablePatient.h" /// for insertInjection to be remove

// --------------------------------------------------------------------
namespace syd {

  /// This database manages a set of 'standard' tables: Patient, Injection etc.
  class StandardDatabase: public Database {

  public:

    // -----------------------------------------------------------
    /// Generic element insertion.
    virtual TableElementBase * InsertFromArg(const std::string & table_name, std::vector<std::string> & arg);

    /// Insert a new Injection (check patient before etc)
    syd::Injection * InsertInjection(std::vector<std::string> & arg);

    /// Insert a new Patient (create folder)
    syd::Patient * InsertPatient(std::vector<std::string> & arg);
    // -----------------------------------------------------------


    // -----------------------------------------------------------
    /// Overload to also delete the files
    virtual bool DeleteCurrentList();

    /// Add a filename to the list of file that will be deleted
    void AddFileToDelete(std::string & f) { list_of_files_to_delete_.push_back(f); }
    // -----------------------------------------------------------


    // -----------------------------------------------------------
    /// Return the injection for the given patient where radionuclide match the arg. Fail if not found
    syd::Injection FindInjectionByNameOrId(const Patient & patient, const std::string & arg);

    /// Find the tag by the label, or create it.
    syd::Tag FindOrInsertTag(const std::string & label, const std::string & description);
    // -----------------------------------------------------------


    // -----------------------------------------------------------
    /// Return the folder where the dicomserie are stored
    std::string GetAbsoluteFolder(const DicomSerie & serie);

    /// Create the folder for a serie (date/modality)
    void CreateAbsoluteFolder(const DicomSerie & serie);

    /// Return the folder where the patient images are stored
    std::string GetAbsoluteFolder(const Patient & patient);

    /// Return the folder where the patient images are stored (relative to the database folder)
    std::string GetRelativeFolder(const Patient & patient);

    /// Return the absolute path of the file
    std::string GetAbsolutePath(const File & file);

    /// Return the absolute path of the image
    std::string GetAbsolutePath(const Image & image);

    /// Return the absolute folder of the image (path without filename)
    std::string GetAbsoluteFolder(const Image & image);
    // -----------------------------------------------------------



    // Below : TODO
    /// Insert a new Timepoint
    syd::Timepoint InsertTimepoint(syd::Injection & injection,
                                   syd::Tag & tag,
                                   std::vector<syd::DicomSerie> & dicoms,
                                   bool replaceTimepointFlag);

    /// Specific Dump for Timepoint
    virtual void DumpTimepoint(const std::vector<std::string> & args, std::ostream & os, bool verboseFlag=false);


  protected:
    /// Insert the tables
    virtual void CreateTables();

    /// List of filenames that will be deleted
    std::vector<std::string> list_of_files_to_delete_;

  }; // class StandardDatabase

  #include "sydStandardDatabase.txx"

} // namespace syd

// --------------------------------------------------------------------

#endif
