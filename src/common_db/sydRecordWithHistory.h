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

#ifndef SYDRECORDWITHHISTORY_H
#define SYDRECORDWITHHISTORY_H

// syd
#include "sydRecordHistory.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object abstract pointer(std::shared_ptr)
  class RecordWithHistory {
  public:

    /// Define pointer type
    typedef std::shared_ptr<RecordWithHistory> pointer;

    /// Define vectortype
    typedef std::vector<pointer> vector;

    /// Store the history. It is 'mutable' because is changed in the const Callback.
    mutable syd::RecordHistory::pointer history;

    virtual void InitTable(syd::PrintTable & table) const;
    virtual void DumpInTable(syd::PrintTable & table) const;
    virtual void DumpInTable(syd::PrintTable2 & table) const;

    virtual void Callback(odb::callback_event,
                          odb::database & odb,
                          syd::Database * db) const;

  protected:
    RecordWithHistory();

  };

} // end namespace
// --------------------------------------------------------------------

#endif
