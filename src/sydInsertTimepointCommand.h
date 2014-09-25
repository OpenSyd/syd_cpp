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
#include "sydTimepointsDatabase.h"
#include "sydDicomCommon.h"

// --------------------------------------------------------------------
namespace syd {

  class InsertTimepointCommand: public syd::DatabaseCommand
  {
  public:

    InsertTimepointCommand(std::string db1, std::string db2);
    InsertTimepointCommand(syd::ClinicDatabase * db1, syd::TimepointsDatabase  * db2);
    ~InsertTimepointCommand();

    void set_ct_selection_patterns(std::string s);

    void InsertTimepoint(std::vector<std::string> serie_ids);
    void InsertTimepoint(Serie serie);

  protected:
    void Initialization();

    syd::ClinicDatabase * db_;
    syd::TimepointsDatabase * tpdb_;
    std::vector<IdType> serie_ids_;
    Patient patient_;
    std::vector<std::string> ct_selection_patterns_;
  };


}  // namespace syd
// --------------------------------------------------------------------

#endif
