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

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::RecordHistory") callback(Callback)
  /// Store information about the history for a RecordHistory
  class RecordHistory: public syd::Record {
  public:

    virtual ~RecordHistory() { DD("destructor RecordHistory"); }

    std::string insertion_date;
    std::string update_date;

    // ------------------------------------------------------------------------
    TABLE_DEFINE(RecordHistory, syd::RecordHistory);
    // ------------------------------------------------------------------------

    /// Write the element as a string
    virtual std::string ToString() const;

    void Set(const syd::Database * db, const std::vector<std::string> & arg);

    virtual void DumpInTable(syd::PrintTable2 & table) const;

  protected:
    RecordHistory();

  }; // end of class


} // end namespace
// --------------------------------------------------------------------

#endif
