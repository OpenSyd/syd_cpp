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

#ifndef SYDTABLET_H
#define SYDTABLET_H

// syd
#include "sydCommon.h"

// // odb
// #include <odb/database.hxx>
// #include <odb/transaction.hxx>
// #include <odb/sqlite/database.hxx>
// #include <odb/sqlite/tracer.hxx>
// #include <odb/sqlite/statement.hxx>
// #include <odb/schema-catalog.hxx>

// --------------------------------------------------------------------
namespace syd {

  //  class Database;

  template< typename T >
  struct table_name{
    static char* name;
  };


  /// Base class for the functions that are common to all tables
  template<class T>
  class TableT {
  public:
    TableT() { }

    virtual std::string GetTableName()  = 0;
    virtual ~TableT() {}

    //virtual void copy(const TableT & t) { table_name = t.table_name; }
    virtual void SetTableHeader(PrintTable & table) = 0;
    virtual void Dump(PrintTable & table) = 0;

    virtual bool Delete(T & t) {
      DD("Delete in TableT");
      return true;
    }


    // virtual bool Delete(IdType id)=0;
    // virtual void DumpTable(std::ostream & os)=0;

  // private:
  //   std::string table_name;
  //   static TableT * singleton_
    ;
    //    odb::sqlite::database * db_;
  };

} // end namespace
// --------------------------------------------------------------------

#endif
