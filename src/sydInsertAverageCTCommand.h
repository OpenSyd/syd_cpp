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

#ifndef SYDCROPCTCOMMAND_H
#define SYDCROPCTCOMMAND_H

// syd
#include "sydDatabaseCommand.h"
#include "sydClinicDatabase.h"
#include "sydStudyDatabase.h"
#include "sydDicomCommon.h"
#include "sydImage.h"

// --------------------------------------------------------------------
namespace syd {

  class InsertAverageCTCommand: public syd::DatabaseCommand
  {
  public:

    InsertAverageCTCommand(std::string d);
    InsertAverageCTCommand(StudyDatabase * d);
    ~InsertAverageCTCommand();

    virtual void Run(std::string patient_name, std::vector<std::string> & args);
    void Run(Patient & patient);

  protected:
    void Initialization();
    std::shared_ptr<syd::ClinicDatabase> cdb_;
    std::shared_ptr<syd::StudyDatabase>  sdb_;
  };

} // end namespace
// --------------------------------------------------------------------

#endif
