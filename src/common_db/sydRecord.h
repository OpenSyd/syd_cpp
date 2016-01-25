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

#ifndef SYDRECORD_H
#define SYDRECORD_H

// syd
#include "sydCommon.h"
#include "sydPrintTable.h"
#include "sydCheckResult.h"
//#include "sydTable.h"
#include "sydVersion.h"

// odb
#include <odb/callback.hxx>
#include <odb/core.hxx>

// --------------------------------------------------------------------
namespace syd {

  class Database;
  //  class RecordHistory;
  //  struct RecordStat;

  /// Base class for all record (or element, or row) in a table
#pragma db object abstract pointer(std::shared_ptr) callback(Callback)
  class Record {
  public:

#pragma db id auto
    /// Main key (automated, unique)
    IdType id;

    /// Define pointer type
    typedef std::shared_ptr<Record> pointer;

    /// Define vectortype
    typedef std::vector<pointer> vector;

    /// Virtual destructor
    virtual ~Record() { }

    /// Return the name of the table
    virtual std::string GetTableName() const = 0;
    virtual std::string GetSQLTableName() const = 0;
    virtual std::string GetInheritTableName() const = 0;
    static std::string GetStaticTableName() { return "Record"; }
    static std::string GetStaticSQLTableName() { return "syd::Record"; }
    static std::string GetStaticInheritTableName() { return "toto"; }
    static void InitInheritance() { }

    /// Set the values of the fields from some string.
    virtual void Set(const syd::Database * db, const std::vector<std::string> & args);

    /// Initialise a PrintTable
    virtual void InitTable(syd::PrintTable & table) const;

    /// Add a line in the given PrintTable
    virtual void DumpInTable(syd::PrintTable & table) const;

    /// Use to write the element as a string (must be overloaded)
    virtual std::string ToString() const = 0;

    /// Default function to print an element (must be inline here).
    friend std::ostream& operator<<(std::ostream& os, const Record & p) {
      os << p.ToString();
      return os;
    }

    /// Default function to print a pointer to an element (must be inline here).
    template<class R>
    friend std::ostream& operator<<(std::ostream& os, const std::shared_ptr<R> p) {
      // FIXME      if (p.get() == 0) os << "[NULL]";
      os << p->ToString();
      return os;
    }

    virtual void Callback(odb::callback_event, odb::database&) const;
    virtual void Callback(odb::callback_event, odb::database&);

    /// Check the record. Usually check the file on disk
    virtual syd::CheckResult Check() const;

    // FIXME
    static std::map<std::string, std::vector<std::string> > inherit_sql_tables_map_;

  protected:
    /// This default constructor allow to oblige class that inherit
    /// from Record to not have default constructor
    Record();

    /// Return true if the record is equal (same id here); (protected
    /// to avoid use with generic pointer)
    virtual bool IsEqual(const pointer p) const;

    /// This field will store a pointer to the db and is not save in
    /// the db (transient). Moreover, we need to set this pointer at
    /// the creation of the object (load or persist). odb call the
    /// const version of the callback, so to be able to change this
    /// pointer, we declare it mutable.
#pragma db transient
    mutable syd::Database * db_;

    // Search for the sydDatebase from the odb::database (slow, but
    // call every new object). It is declare const, but will change
    // the db_ (mutable)
    virtual void SetDatabasePointer(odb::callback_event event, odb::database & d) const;

  }; // end of class


  /// odb::access is needed for polymorphism
  /// Also define a view that can count the nb of elements in a table
  /// http://comments.gmane.org/gmane.comp.lang.c%2B%2B.odb.user/1602

#define TABLE_DEFINE_I(TABLE_NAME, SQL_TABLE_NAME, INHERIT_TABLE_NAME)    \
  typedef std::shared_ptr<TABLE_NAME> pointer;                          \
  typedef std::vector<pointer> vector;                                  \
  friend class odb::access;                                             \
  virtual std::string GetTableName() const { return #TABLE_NAME; }      \
  virtual std::string GetSQLTableName() const { return #SQL_TABLE_NAME; } \
  virtual std::string GetInheritTableName() const { return #INHERIT_TABLE_NAME; } \
  static std::string GetStaticTableName() { return #TABLE_NAME; }       \
  static std::string GetStaticSQLTableName() { return #SQL_TABLE_NAME; } \
  static std::string GetStaticInheritTableName() { return #INHERIT_TABLE_NAME; } \
  static void InitInheritance() {                                       \
    INHERIT_TABLE_NAME::InitInheritance();                              \
    inherit_sql_tables_map_[#TABLE_NAME].push_back(INHERIT_TABLE_NAME::GetStaticSQLTableName()); } \
  static pointer New() { return pointer(new TABLE_NAME); }              \
  struct TABLE_NAME##_count                                             \
  {                                                                     \
    int count;                                                          \
  };                                                                    \
  PRAGMA_DB(view(TABLE_NAME##_count) object(TABLE_NAME))                \
  PRAGMA_DB(member(TABLE_NAME##_count::count)                           \
            column("count(" + TABLE_NAME::id + ")"))

#define TABLE_DEFINE(TABLE_NAME, SQL_TABLE_NAME)        \
  TABLE_DEFINE_I(TABLE_NAME, SQL_TABLE_NAME, syd::Record)


#define TABLE_DECLARE_MANDATORY_FUNCTIONS(TABLE_NAME)   \
  virtual std::string ToString() const;                 \
  virtual bool IsEqual(const pointer p) const;

#define TABLE_DECLARE_OPTIONAL_FUNCTIONS(TABLE_NAME)                    \
  virtual void Set(const syd::Database * db, const std::vector<std::string> & args);

  /// To be define in db.cxx (and declared in db.h) to allow fast
  /// computation of the nb of elements in a table.
  /// http://comments.gmane.org/gmane.comp.lang.c%2B%2B.odb.user/1602
#define TABLE_GET_NUMBER_OF_ELEMENTS(TABLE_NAME)                        \
  template<>                                                            \
  long syd::Database::GetNumberOfElements<TABLE_NAME>() const           \
  {                                                                     \
    typedef odb::query<TABLE_NAME> query;                               \
    odb::transaction t (odb_db_->begin());                              \
    TABLE_NAME::TABLE_NAME##_count ec(odb_db_->query_value<TABLE_NAME::TABLE_NAME##_count> \
                                      (odb::query<TABLE_NAME>::id != 0)); \
    auto c = ec.count;                                                  \
    t.commit ();                                                        \
    return c;                                                           \
  }

} // end namespace
// --------------------------------------------------------------------

#endif
