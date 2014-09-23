/*=========================================================================
  Copyright 2014 <David Sarrut>
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

  ============================================================================*/

#ifndef SRC_SYDCHECKINTEGRITYCOMMAND_H_
#define SRC_SYDCHECKINTEGRITYCOMMAND_H_

// std
#include <string>

// syd
#include "core/sydDatabaseCommand.h"
#include "db/clinicaltrial/sydClinicalTrialDatabase.h"
#include "core/sydDicomCommon.h"

// --------------------------------------------------------------------
namespace syd {

  class CheckIntegrityCommand: public syd::DatabaseCommand {
   public:
    CheckIntegrityCommand();
    ~CheckIntegrityCommand();

    virtual void AddDatabase(syd::Database * d);
    virtual void SetArgs(char ** inputs, int n);
    virtual void Run();

   protected:
    void CheckFile(OFString filename);
    syd::ClinicalTrialDatabase * db_;
    std::string patient_name_;
    Patient patient_;
  };

}  // namespace syd
// --------------------------------------------------------------------

#endif  // SRC_SYDCHECKINTEGRITYCOMMAND_H_
