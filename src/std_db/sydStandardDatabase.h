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
#include "sydImage-odb.hxx"
#include "sydRoiType-odb.hxx"
#include "sydRoiMaskImage-odb.hxx"

// --------------------------------------------------------------------
namespace syd {

  /// This database manages a set of 'standard' tables: Patient, Injection etc.
  class StandardDatabase: public Database {

  public:

    // -----------------------------------------------------------
    /// Generic element insertion.
    virtual TableElementBase * InsertFromArg(const std::string & table_name,
                                             std::vector<std::string> & arg);

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
    /// Return the patient by name or study_id. Fail if not found.
    syd::Patient FindPatientByNameOrStudyId(const std::string & arg);

    /// Return the injection for the given patient where radionuclide match the arg. Fail if not found
    syd::Injection FindInjectionByNameOrId(const Patient & patient,
                                           const std::string & arg);

    /// Return dicom series that match the patterns
    void FindDicom(const syd::Patient & patient,
                   const std::vector<std::string> & patterns,
                   std::vector<syd::DicomSerie> & series);

    /// Return images that match the patterns
    void FindImage(const syd::Patient & patient,
                   const std::vector<std::string> & patterns,
                   std::vector<syd::Image> & images);

    /// Return the roitype from its name
    syd::RoiType FindRoiType(const std::string & name);

    /// Return the roi --> depend on the time could be several
    syd::RoiMaskImage FindRoiMaskImage_TODO(const syd::Patient & patient, const syd::RoiType & roitype);

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


    //    void AddTag(syd::Image & image, syd::Tag & tag);

    // Below : TODO

    // /// Simple default Dump overwritten here to select special case DumpDicom.
    // virtual void Dump(const std::vector<std::string> & args, std::ostream & os);

    /// Insert a new Timepoint
    syd::Timepoint InsertTimepoint(syd::Injection & injection,
                                   syd::Tag & tag,
                                   std::vector<syd::DicomSerie> & dicoms,
                                   bool replaceTimepointFlag);

    /// Specific Dump for Timepoint
    virtual void DumpTimepoint(const std::vector<std::string> & args, std::ostream & os, bool verboseFlag=false);


    /// What to do when a element is planned to be deleted
    // virtual void OnDelete(const std::string & table_name, TableElementBase * e);

    // /// What to do when an Image will be deleted
    // void OnDeleteImage(syd::Image & e);

    // /// What to do when a File will be deleted
    // void OnDeleteFile(syd::File & e);

    // /// What to do when a Tag will be deleted
    // void OnDeleteTag(syd::Tag & e);
    // void OnDeleteDicomFile(syd::DicomFile & e);
    // void OnDeleteDicomSerie(syd::DicomSerie & e);

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
