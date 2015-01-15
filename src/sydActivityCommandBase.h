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

#ifndef SYDACTIVITYCOMMANDBASE_H
#define SYDACTIVITYCOMMANDBASE_H

// syd
#include "sydDatabaseCommand.h"
#include "sydActivityDatabase.h"
#include "sydActivity_ggo.h"

// --------------------------------------------------------------------
namespace syd {

  class ActivityCommandBase: public syd::DatabaseCommand
  {
  public:

    ActivityCommandBase();
    ~ActivityCommandBase();

    typedef float PixelType;
    typedef signed short int CTPixelType;
    typedef uchar MaskPixelType;
    typedef itk::Image<PixelType, 3> ImageType;
    typedef itk::Image<CTPixelType, 3> CTImageType;
    typedef itk::Image<MaskPixelType, 3> MaskImageType;

    void Initialize(std::shared_ptr<syd::ActivityDatabase> db,
                    args_info_sydActivity & args_info);
    void Initialize(std::shared_ptr<syd::ActivityDatabase> db);
    void Run(std::vector<std::string> args);
    void Run(const Patient & patient, std::vector<std::string> args);

  protected:
    virtual void SetOptions(args_info_sydActivity & args_info) = 0;
    virtual void Run(const Patient & p, const RoiType & roitype,
                     std::vector<std::string> & args) = 0;
    void GetOrCreateTimeActivities(const Patient & patient,
                                   const RoiType & roitype,
                                   std::vector<Timepoint> & timepoints,
                                   std::vector<TimeActivity> & timeactivities);
    std::shared_ptr<syd::ClinicDatabase>    cdb_;
    std::shared_ptr<syd::StudyDatabase>     sdb_;
    std::shared_ptr<syd::ActivityDatabase>  adb_;

  };

}  // namespace syd
// --------------------------------------------------------------------

#endif
