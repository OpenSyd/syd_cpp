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
#include "sydClinicDatabase.h"
#include "Timepoint-odb.hxx"
#include "RawImage-odb.hxx"
#include "RoiMaskImage-odb.hxx"

// Manage a list of timepoint with associated raw image. Need a
// pointer to a ClinicDatabase because each image is linked with a
// Serie and a Patient from this ClinicDatabase;
// --------------------------------------------------------------------
namespace syd {

  class StudyDatabase: public Database {

  public:
    StudyDatabase(std::string name, std::string param);
    ~StudyDatabase();

    SYD_INIT_DATABASE(StudyDatabase);

    // Query
    std::string GetPath(const Patient & p);
    std::string GetOrCreatePath(const Patient & p);
    std::string GetRoiPath(const Patient & p);
    std::string GetOrCreateRoiPath(const Patient & p);
    std::string GetRegistrationOutputPath(Timepoint ref, Timepoint mov);
    std::string GetImagePath(const RawImage & image);
    std::string GetImagePath(const RoiMaskImage & roi);
    std::string GetImagePathFromRawImageId(IdType id);
    std::string GetSpectImagePath(const Timepoint & timepoint);
    Patient GetPatient(const Timepoint & timepoint);
    RoiMaskImage GetRoiMaskImage(const Timepoint & timepoint, std::string roiname);
    // FIXME put vector in args
    std::vector<RoiMaskImage> GetRoiMaskImages(const Timepoint & timepoint, std::string roiname);
    std::vector<RoiType> GetRoiTypes(std::string roiname);
    RoiType GetRoiType(const RoiMaskImage & roimaskimage);
    void GetSpectImageFilenames(const Patient & patient, std::vector<std::string> & filenames);
    void GetTimepoints(const Patient & patient, std::vector<Timepoint> & timepoints);
    void GetTimepointsByNumber(const Patient & patient, const std::string & query, std::vector<Timepoint> & timepoints);

    // other functions
    double Convert_Counts_to_MBq(const Timepoint & timepoint, double v);

    // Dump information
    virtual void Dump(std::ostream & os, std::vector<std::string> & args);
    virtual void DumpRoi(std::ostream & os, std::vector<std::string> & args);
    virtual void DumpRoi(std::ostream & os);
    virtual void CreateDatabase();

    Timepoint NewTimepoint(const Serie & spect_serie, const Serie & ct_serie);
    RoiMaskImage NewRoiMaskImage(const Timepoint & timepoint, const RoiType & roitype);
    RawImage NewRawImage(const Patient & patient);


    /// Limitation : only float image pixeltype by default (FIXME)
    /// filename as input, could be change to dose.mhd uncert/mhd ?
    RawImage InsertDoseImage(const Patient & patient,
                             std::string filename,
                             std::string uncertainty,
                             double scale);
    RawImage InsertTagImage(const Patient & patient,
                            std::string tag,
                            std::string filename,
                            double scale=1.0);



    void UpdateTimepoint(const Serie & spect_serie, const Serie & ct_serie, Timepoint & t);
    void UpdateRoiMaskImage(RoiMaskImage & roi);
    void UpdateRoiMaskImageVolume(RoiMaskImage & roi);
    void UpdateAverageCTImage(RawImage & rawimage);

    void ConvertCTDicomToImage(const Timepoint & t);
    void ConvertSpectDicomToImage(const Timepoint & t);
    void ConvertDicomToImage(const Timepoint & t);
    void CopyFilesFrom(std::shared_ptr<StudyDatabase> in_db, const Timepoint & in, Timepoint & out);

    void UpdateMD5(RawImage & image);
    void UpdatePathAndRename(const Timepoint & timepoint, bool rename_flag=true);
    void UpdateNumberAndRenameFiles(IdType patient_id);

    virtual void CheckIntegrity(std::vector<std::string> & args);
    void CheckIntegrity(const RawImage & image);
    void CheckIntegrity(const Timepoint & timepoint);
    void CheckIntegrity(const Patient & patient);

    // Print
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
