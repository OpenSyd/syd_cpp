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

#ifndef SYDCLEARSERIESCOMMAND_H
#define SYDCLEARSERIESCOMMAND_H

// syd
#include "sydDatabaseCommand.h"
#include "sydClinicalTrialDatabase.h"
#include "sydDicomCommon.h"

// --------------------------------------------------------------------
namespace syd {

  class ClearSeriesCommand: public syd::DatabaseCommand
  {
  public:

    virtual void SetArgs(char ** inputs, int n);
    virtual void Run();

  protected:
    syd::ClinicalTrialDatabase * db;
    std::string patient_name_;
    Patient patient_;
  };


} // end namespace
// --------------------------------------------------------------------

#endif
