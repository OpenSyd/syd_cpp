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

#ifndef SYDSTUDYSDATABASE_H
#define SYDSTUDYSDATABASE_H

// syd
#include "sydDatabase.h"
#include "sydImage.h"
#include "Timepoint-odb.hxx"
#include "RawImage-odb.hxx"
#include "RoiMaskImage-odb.hxx"
#include "sydClinicDatabase.h"

// Manage a list of timepoint with associated raw image. Uses 3 tables
// (Timepoint RawImage RoiMaskImage).  Need a pointer to a
// ClinicDatabase because each image is linked with a Serie and a
// Patient from this ClinicDatabase;
// --------------------------------------------------------------------
namespace syd {

  class StudyDatabase: public Database {

  public:
    StudyDatabase(std::string name, std::string param);
    ~StudyDatabase();

    SYD_INIT_DATABASE(StudyDatabase);

    // Dump information
    virtual void Dump(std::ostream & os, std::vector<std::string> & args);
    virtual void CheckIntegrity(std::vector<std::string> & args) { DD("todo"); exit(0); }
    virtual void CreateDatabase();

    void InsertTimepoint(Timepoint & t, RawImage & spect, RawImage & ct);
    void InsertRoiMaskImage(const Timepoint & timepoint, const RoiType & roitype, RoiMaskImage & roi);

    void CopyFilesTo(const Timepoint & in, std::shared_ptr<StudyDatabase> out_db, Timepoint & out);
    void CopyFilesTo(const RawImage & in, std::shared_ptr<StudyDatabase> out_db, RawImage & out);

    bool FilesExist(Timepoint t); // FIXME
    bool CheckMD5(Timepoint t); // FIXME
    void UpdateMD5(Timepoint t); //FIXME
    void UpdateMD5(RawImage & image);
    std::string GetRegistrationOutputPath(Timepoint ref, Timepoint mov); // FIXME

    void UpdateNumberAndRenameFiles(IdType patient_id);
    void ConvertDicomToImage(const Timepoint & t);

    std::string GetPath(const Patient & p);
    std::string GetOrCreatePath(const Patient & p);
    std::string GetRoiPath(const Patient & p);
    std::string GetOrCreateRoiPath(const Patient & p);
    std::string GetPath(const RawImage & i);
    std::string GetImagePath(IdType id);
    std::string GetImagePath(const RoiMaskImage & roi);
    // Timepoint GetTimepoint(const RawImage & image);
    // Patient GetPatient(const RawImage & image);
    // Patient GetPatient(const RoiMaskImage & image);
    Patient GetPatient(const Timepoint & timepoint);

    void UpdateImageFilenames(const Timepoint & t);

    std::string Print(const Timepoint & t);
    std::string Print(const RawImage & t);
    std::string Print(const Patient & p, int level=0);

    std::shared_ptr<ClinicDatabase> get_clinical_database() { return cdb_; }

  protected:
    std::shared_ptr<ClinicDatabase> cdb_;
  }; // end class


} // end namespace
// --------------------------------------------------------------------

#endif
