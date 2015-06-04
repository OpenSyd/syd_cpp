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

#ifndef SYDACTIVITYDATABASE_H
#define SYDACTIVITYDATABASE_H

// syd
#include "sydImage.h"
#include "sydStudyDatabase.h"
#include "Activity-odb.hxx"
#include "TimeActivity-odb.hxx"

// itk
#include <itkLabelStatisticsImageFilter.h>

// Manage a list of computation about activities and time-integrated
// activities, associated to a StudyDatabase
// --------------------------------------------------------------------
namespace syd {

  class ActivityDatabase: public Database {

  public:
    ActivityDatabase(std::string name, std::string param);
    ~ActivityDatabase();

    SYD_INIT_DATABASE(ActivityDatabase);

    typedef float PixelType;
    typedef itk::Image<PixelType, 3> ImageType;
    typedef uchar MaskPixelType;
    typedef itk::Image<MaskPixelType, 3> MaskImageType;

    // Dump information
    virtual void Dump(std::ostream & os, std::vector<std::string> & args);
    void Dump(std::ostream & os, const std::string & cmd, const Patient & patient, std::vector<std::string> & args);
    void DumpLambda(std::ostream & os, std::vector<Patient> & patients, std::vector<std::string> & args);
    void DumpTimeIntegratedActivities(std::ostream & os, std::vector<Patient> & patients, std::vector<std::string> & args, std::string unit);
    void DumpWeight(std::ostream & os, std::vector<Patient> & patients, std::vector<std::string> & args);
    void DumpDose(std::ostream & os, std::vector<Patient> & patients, std::vector<std::string> & args);
    void DumpArticle(std::ostream & os, std::vector<Patient> & patients, std::vector<std::string> & args);

    virtual void CheckIntegrity(std::vector<std::string> & args);
    virtual void CreateDatabase();

    Activity NewActivity(const Patient & patient, const RoiType & roitype);
    TimeActivity NewTimeActivity(const Timepoint & t, const RoiMaskImage & roi);

    std::shared_ptr<ClinicDatabase> get_clinical_database() { return cdb_; }
    std::shared_ptr<StudyDatabase> get_study_database() { return sdb_; }

    void set_mean_radius(double v) { mean_radius_ = v; }
    double get_mean_radius() const { return mean_radius_; }

    bool GetRoiMaskImage(Activity & activity, RoiMaskImage & roimask);
    double Get_CountByMM3_in_MBqByCC(double v);
    double Get_CountByMM3_in_MBqByKG(Activity & activity, double v);
    double Get_CountByMM3_in_MBqByKG(TimeActivity & timeactivity, double v);
    double Get_CountByMM3_in_PercentInjectedActivityByKG(Activity & activity, double v);
    double Get_CountByMM3_in_PercentInjectedActivityByKG(TimeActivity & timeactivity, double v);

  protected:
    std::shared_ptr<ClinicDatabase> cdb_;
    std::shared_ptr<StudyDatabase>  sdb_;
    double mean_radius_;

  }; // end class


  //http://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
  template <typename T>
  std::vector<size_t> sort_indexes(const std::vector<T> &v) {

    // initialize original index locations
    std::vector<size_t> idx(v.size());
    for (size_t i = 0; i != idx.size(); ++i) idx[i] = i;

    // sort indexes based on comparing values in v
    std::sort(idx.begin(), idx.end(),
              [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});
    return idx;
  }


} // end namespace
// --------------------------------------------------------------------

#endif