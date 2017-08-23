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
#include "sydRecordTraitsBase.h"

// --------------------------------------------------------------------
namespace syd {

  /// WARNING: this class does not inherit from syd::Record. It is used for
  /// composition such that class may multiple inherit from syd::Record and
  /// other syd::RecordWithSomething classes

#pragma db object abstract pointer(std::shared_ptr)
  class RecordWithHistory {
  public:

    /// Store the history. It is 'mutable' because is changed in the const Callback.
    mutable syd::RecordHistory::pointer history;

    /// Specific case for RecordWithHistory (composition not inheritance)
    typedef std::shared_ptr<syd::RecordWithHistory> pointer;
    typedef std::function<bool(pointer a, pointer b)> CompareFunction;
    typedef std::map<std::string, CompareFunction> CompareFunctionMap;
    static void BuildMapOfSortFunctions(CompareFunctionMap & map);

    /// Specific case for RecordWithHistory (composition not inheritance)
    typedef std::function<std::string(pointer)> SpecificFieldFunc;
    typedef std::map<std::string, SpecificFieldFunc> FieldFunctionMap;
    static void BuildMapOfFieldsFunctions(FieldFunctionMap & map);

    typedef syd::RecordTraitsBase::FieldMapType FieldMapType;
    static void BuildFields(const syd::Database * db, FieldMapType & map);

    void SetPrintHistoryFlag(bool b) { print_history_flag_ = b; }

    virtual void Callback(odb::callback_event,
                          odb::database & odb,
                          syd::Database * db) const;

  protected:
    RecordWithHistory();

    /// Not stored in the db //FIXME to remove ?
 #pragma db transient
   bool print_history_flag_;

  };


} // end namespace
// --------------------------------------------------------------------

#endif
