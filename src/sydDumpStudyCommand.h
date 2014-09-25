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

#ifndef SYDDUMPSTUDYCOMMAND_H
#define SYDDUMPSTUDYCOMMAND_H

// syd
#include "sydDatabaseCommand.h"
#include "sydClinicDatabase.h"
#include "sydTimepointsDatabase.h"
#include "sydDicomCommon.h"

// --------------------------------------------------------------------
namespace syd {

  class DumpStudyCommand: public syd::DatabaseCommand
  {
  public:

    DumpStudyCommand(std::string db1, std::string db2);
    DumpStudyCommand(syd::ClinicDatabase * db1, syd::TimepointsDatabase  * db2);
    ~DumpStudyCommand();

    virtual void Dump(std::string patient_name);

  protected:
    void Initialization();
    syd::ClinicDatabase * db_;
    syd::TimepointsDatabase * tpdb_;

  }; // class DumpStudyCommand

} // namespace syd
// --------------------------------------------------------------------

#endif
