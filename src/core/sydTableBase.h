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

#ifndef SYDTABLEBASE_H
#define SYDTABLEBASE_H

// syd
#include "sydCommon.h"
#include "sydTableElement.h"

// odb
#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/sqlite/database.hxx>
#include <odb/sqlite/tracer.hxx>
#include <odb/sqlite/statement.hxx>
#include <odb/schema-catalog.hxx>

// --------------------------------------------------------------------
namespace syd {

  class Database;

  /// Base class for the functions that are common to all tables
  class TableBase {
  public:
    TableBase(odb::sqlite::database * d):db_(d) {}

    virtual TableElement * InsertFromArg(std::vector<std::string> & arg) = 0;
    virtual void DumpTable(std::ostream & os) = 0;
    virtual unsigned int GetNumberOfElements() = 0;

  protected:
    odb::sqlite::database * db_;
  };

} // end namespace
// --------------------------------------------------------------------

#endif
