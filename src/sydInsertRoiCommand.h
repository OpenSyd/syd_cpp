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

#ifndef SYDINSERTROICOMMAND_H
#define SYDINSERTROICOMMAND_H

// syd
#include "sydDatabaseCommand.h"
#include "sydClinicDatabase.h"
#include "sydStudyDatabase.h"
#include "sydDicomCommon.h"
#include "sydImage.h"

// --------------------------------------------------------------------
namespace syd {

  class InsertRoiCommand: public syd::DatabaseCommand
  {
  public:

    InsertRoiCommand(std::string d);
    InsertRoiCommand(StudyDatabase * d);
    ~InsertRoiCommand();

    void Run(std::vector<std::string> & arg);
    void Run(const Patient & patient, const std::vector<std::string> & arg);
    void Run(const Timepoint & timepoint, const RoiType & roitype, std::string filename);

    void set_move_flag(const bool b) { move_flag_ = b; }
    bool get_move_flag() const { return move_flag_; }

  protected:
    void Initialization();
    void Run(const Timepoint & t);
    bool move_flag_;

    std::shared_ptr<syd::ClinicDatabase> cdb_;
    std::shared_ptr<syd::StudyDatabase>  sdb_;
  };
} // end namespace
// --------------------------------------------------------------------

#endif
