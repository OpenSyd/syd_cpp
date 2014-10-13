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

#ifndef SYDINSERTTIMEPOINTCOMMAND_H
#define SYDINSERTTIMEPOINTCOMMAND_H

// syd
#include "sydDatabaseCommand.h"
#include "sydActivityDatabase.h"
#include "sydDicomCommon.h"

// --------------------------------------------------------------------
namespace syd {

  class ActivityCommand: public syd::DatabaseCommand
  {
  public:

    ActivityCommand(std::string db);
    ActivityCommand(syd::ActivityDatabase * db);
    ~ActivityCommand();

    void Run(std::vector<std::string> & args);
    void Run(const Patient & patient, std::vector<std::string> & args);
    void Run(const Timepoint & timepoint, std::vector<std::string> & args);
    void UpdateActivityInRoi(const Timepoint & timepoint,
                             const RoiMaskImage & roi,
                             TimeActivity & timeactivity);

  protected:
    void Initialization();
    std::shared_ptr<syd::ClinicDatabase> cdb_;
    std::shared_ptr<syd::StudyDatabase>  sdb_;
    std::shared_ptr<syd::ActivityDatabase>  adb_;
  };

}  // namespace syd
// --------------------------------------------------------------------

#endif
