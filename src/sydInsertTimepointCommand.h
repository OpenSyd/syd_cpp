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
#include "sydClinicDatabase.h"
#include "sydStudyDatabase.h"
#include "sydDicomCommon.h"

// --------------------------------------------------------------------
namespace syd {

  class InsertTimepointCommand: public syd::DatabaseCommand
  {
  public:

    InsertTimepointCommand(std::string db);
    InsertTimepointCommand(syd::StudyDatabase * db);
    ~InsertTimepointCommand();

    void set_ct_selection_patterns(std::string s);
    void set_ignore_files_flag(bool b) { ignore_files_flag_ = b; }
    bool get_ignore_files_flag() { return ignore_files_flag_; }

    void InsertTimepoint(std::vector<std::string> serie_ids);
    void InsertTimepoint(const Serie & serie);

  protected:
    void Initialization();

    bool ignore_files_flag_;
    std::shared_ptr<syd::ClinicDatabase> cdb_;
    std::shared_ptr<syd::StudyDatabase>  sdb_;
    std::vector<std::string> ct_selection_patterns_;
  };


}  // namespace syd
// --------------------------------------------------------------------

#endif
