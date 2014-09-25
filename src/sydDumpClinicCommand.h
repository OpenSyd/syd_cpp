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

#ifndef SYDDUMPCLINICCOMMAND_H
#define SYDDUMPCLINICCOMMAND_H

// syd
#include "sydDatabaseCommand.h"
#include "sydClinicDatabase.h"
#include "sydDicomCommon.h"

// --------------------------------------------------------------------
namespace syd {

  class DumpClinicCommand: public syd::DatabaseCommand
  {
  public:

    DumpClinicCommand(std::string db);
    DumpClinicCommand(syd::ClinicDatabase * db);
    ~DumpClinicCommand();

    // FIXME to remove
    virtual void SetArgs(char ** inputs, int n) {}
    virtual void Run() {}

    virtual void Dump(std::string cmd, std::string patient_name, std::vector<std::string> & patterns);

    void DumpTimepoints(Patient patient, std::vector<std::string> & patterns);

  protected:
    void Initialization();
    virtual void OpenCommandDatabases() {} // FIXME to remove
    syd::ClinicDatabase * db_;

  }; // class DumpClinicCommand

} // namespace syd
// --------------------------------------------------------------------

#endif
