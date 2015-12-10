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

#ifndef SYDRECORDHISTORY_H
#define SYDRECORDHISTORY_H

// syd
#include "sydRecord.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db model version(1, 1)

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::RecordHistory") callback(Callback)
  /// Store information about the history for a RecordHistory
  class RecordHistory: public syd::Record {
  public:

    //    syd::Record::pointer record; // back pointer ?

    std::string insertion_date;
    std::string update_date;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(RecordHistory);
    TABLE_DECLARE_MANDATORY_FUNCTIONS(RecordHistory);
    // ------------------------------------------------------------------------

    virtual void InitTable(syd::PrintTable & table) const;
    virtual void DumpInTable(syd::PrintTable & table) const;

  protected:
    RecordHistory();

  }; // end of class


} // end namespace
// --------------------------------------------------------------------

#endif
