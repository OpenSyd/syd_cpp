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

#ifndef SYDTABLE_H
#define SYDTABLE_H

// syd
//#include "sydTableBase.h"
#include "sydDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  class Database;

  /// This class is used to encapsulate common functions for a
  /// database table that manage some Record.
  template<class Record>
  class Table: public TableBase {
  public:

    Table():TableBase() {}

    //    static std::string GetTableName() { return Record::GetTableName(); }

    /// Compute the number of elements in this table
    //    virtual unsigned int GetNumberOfElements() const;

    virtual std::shared_ptr<syd::Record> New() const;

    virtual void Insert2(std::shared_ptr<syd::Record> record) const;

    //  protected:
    syd::Database * db_;

  };
} // end namespace


#include "sydTable.txx"


// --------------------------------------------------------------------

#endif
