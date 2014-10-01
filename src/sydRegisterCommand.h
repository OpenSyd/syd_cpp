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

#ifndef SYDREGISTERCOMMAND_H
#define SYDREGISTERCOMMAND_H

// syd
#include "sydDatabaseCommand.h"
#include "sydClinicDatabase.h"
#include "sydStudyDatabase.h"
#include "sydDicomCommon.h"
#include "sydImage.h"

// --------------------------------------------------------------------
namespace syd {

  class RegisterCommand: public syd::DatabaseCommand
  {
  public:

    RegisterCommand(std::string d1, std::string d2);
    RegisterCommand(StudyDatabase * d1, StudyDatabase * d2);
    ~RegisterCommand();

    virtual void Run(std::string patient_name, int a, int b);

    void set_config_filename(std::string s) { config_filename_ = s; }

  protected:
    void Initialization();
    void Run(Timepoint ref, Timepoint mov);

    std::shared_ptr<syd::ClinicDatabase> cdb_;
    std::shared_ptr<syd::StudyDatabase>  in_db_;
    std::shared_ptr<syd::StudyDatabase>  out_db_;
    std::string config_filename_;
  };


} // end namespace
// --------------------------------------------------------------------

#endif
