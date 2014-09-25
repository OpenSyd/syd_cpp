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
#include "sydTimepointsDatabase.h"
#include "sydDicomCommon.h"
#include "sydImage.h"

// --------------------------------------------------------------------
namespace syd {

  class RegisterCommand: public syd::DatabaseCommand
  {
  public:

    RegisterCommand(ClinicDatabase * d1, TimepointsDatabase * d2, TimepointsDatabase * d3);
    RegisterCommand(std::string d1, std::string d2, std::string d3);
    ~RegisterCommand();

    virtual void Run(std::string patient_name, int a, int b);

  protected:
    void Initialization();
    void Run(Timepoint ref, Timepoint mov);

    syd::ClinicDatabase * db_;
    syd::TimepointsDatabase * tpdb_;
    syd::TimepointsDatabase * reg_tpdb_;
    // Patient patient_;
    // int reference_number_;
    // int moving_number_;

  };


} // end namespace
// --------------------------------------------------------------------

#endif
