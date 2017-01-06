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
#include "sydCheckResult.h"
#include "sydVersion.h"
#include "sydRecordTraits.h"

// odb
#include <odb/callback.hxx>
#include <odb/core.hxx>

// --------------------------------------------------------------------
namespace syd {

  class Database;
  class PrintTable;

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

    // ----------------------------------------------------
    /// FIXME
    // MUST BE overloaded
    virtual RecordTraitsBase * traits() const;
    friend class syd::RecordTraits<syd::Record>;
    /// return the name of the table
    virtual std::string GetTableName() const;
    // ----------------------------------------------------

    /// Return the name of the table
    // FIXME will be removed
    //virtual std::string GetTableName() const = 0;
    virtual std::string GetSQLTableName() const { return "nothing_yet"; }
    static std::string GetStaticTableName() { return "Record"; }
    static std::string GetStaticSQLTableName() { return "syd::Record"; }
    static void InitInheritance() { }

    /// Set the values of the fields from some string.
    virtual void Set(const std::vector<std::string> & args);

    /// Add a line in the given PrintTable
    virtual void DumpInTable(syd::PrintTable & table) const;

    /// Use to write the element as a string (must be overloaded)
    virtual std::string ToString() const;

    /// Use to write the element as a string but more shortly than previous
    virtual std::string ToShortString() const { return ToString(); }

    /// Default function to print an element (must be inline here).
    friend std::ostream& operator<<(std::ostream& os, const Record & p) {
      os << p.ToShortString();
      return os;
    }

    /// Default function to print a pointer to an element (must be inline here).
    template<class R>
      friend std::ostream& operator<<(std::ostream& os, const std::shared_ptr<R> p) {
      os << p->ToShortString();
      return os;
    }

    virtual void Callback(odb::callback_event, odb::database&) const;
    virtual void Callback(odb::callback_event, odb::database&);

    /// Check the record. Usually check the file on disk
    virtual syd::CheckResult Check() const;

    /// Return true if the record is persistent (in the db)
    bool IsPersistent() const;
    void CheckIfPersistant() const;

    // FIXME
    static std::map<std::string, std::vector<std::string> > inherit_sql_tables_map_;

    /// Return the db where this record is stored
    syd::Database * GetDatabase() const;

    /// Return the db where this record is stored and consider it as a DatabaseType
    template<class DatabaseType>
      DatabaseType * GetDatabase() const;

    /// Get (build) the list of default filed getter

    //    typedef std::function<std::string(syd::Record::pointer)> GetFieldFunction;
    //    virtual void SetDefaultFields(std::map<std::string, syd::Record::GetFieldFunction> & map) const;

  protected:
    /// This default constructor allow to oblige class that inherit
    /// from Record to not have default constructor
    Record();

    /// This field will store a pointer to the db and is not save in
    /// the db (transient). We set this pointer at the creation of the
    /// object (load or persist). odb call the const version of the
    /// callback, so to be able to change this pointer, we declare it
    /// mutable.
#pragma db transient
    mutable syd::Database * db_;

    /// Set the db
    virtual void SetDatabasePointer(syd::Database * db);

    // Search for the sydDatebase from the odb::database (slow, but
    // call every new object). It is declare const, but will change
    // the db_ (mutable)
    virtual void SetDatabasePointer(odb::callback_event event, odb::database & d) const;

  }; // end of class
  // --------------------------------------------------------------------



  // --------------------------------------------------------------------
  /// Default function to check equality (with tostring)
  bool IsEqual(const syd::Record::pointer r1, const syd::Record::pointer r2);
  template<class R>
    inline bool operator==(const std::shared_ptr<R> & left,
                           const std::shared_ptr<R> & right)
    { return (syd::IsEqual(left, right)); }
  template<class R>
    inline bool operator!=(const std::shared_ptr<R> & left,
                           const std::shared_ptr<R> & right)
    { return !(left == right); }

#include "sydRecord.txx"
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  /// odb::access is needed for polymorphism
#define TABLE_DEFINE_I(TABLE_NAME, SQL_TABLE_NAME, INHERIT_TABLE_NAME)  \
  typedef std::shared_ptr<TABLE_NAME> pointer;                          \
  typedef std::vector<pointer> vector;                                  \
  friend class odb::access;                                             \
  friend class syd::RecordTraits<TABLE_NAME>;                           \
  virtual RecordTraitsBase * GetTraits() const {                        \
    return RecordTraits<TABLE_NAME>::GetTraits(#TABLE_NAME);            \
  }                                                                     \
  virtual std::string GetTableName() const { return #TABLE_NAME; }      \
  virtual std::string GetSQLTableName() const { return #SQL_TABLE_NAME; } \
  static std::string GetStaticTableName() { return #TABLE_NAME; }       \
  static std::string GetStaticSQLTableName() { return #SQL_TABLE_NAME; } \
  static void InitInheritance() {                                       \
    INHERIT_TABLE_NAME::InitInheritance();                              \
    inherit_sql_tables_map_[#TABLE_NAME].push_back(INHERIT_TABLE_NAME::GetStaticSQLTableName());} \
  static pointer New() { return pointer(new TABLE_NAME); }

#define TABLE_DEFINE(TABLE_NAME, SQL_TABLE_NAME)          \
  TABLE_DEFINE_I(TABLE_NAME, SQL_TABLE_NAME, syd::Record)


  /// Macros REQUIRED in EVERY table
  /// First macro: in the class definition, public
  /// Second macro: after the class definition in the header
  /// Third macro: in the .cxx file

#define DEFINE_TABLE_CLASS(TABLE_NAME)                                \
  friend class odb::access;                                           \
  friend syd::RecordTraits<TABLE_NAME>;                               \
  typedef std::shared_ptr<TABLE_NAME> pointer;                        \
  typedef std::vector<pointer> vector;                                \
  /* FIXME TO REMOVE*/                                                \
  virtual std::string GetSQLTableName() const { return #TABLE_NAME; } \
  static std::string GetStaticTableName() { return #TABLE_NAME; }     \
  static std::string GetStaticSQLTableName() { return #TABLE_NAME; }  \

#define DEFINE_TABLE_HEADER(TABLE_NAME)                                 \
  template<> syd::RecordTraitsBase * RecordTraits<TABLE_NAME>::GetTraits();

#define DEFINE_TABLE_IMPL(TABLE_NAME)                                 \
  namespace syd {                                                     \
    template<>                                                        \
      syd::RecordTraitsBase * RecordTraits<TABLE_NAME>::GetTraits() { \
      return syd::RecordTraits<TABLE_NAME>::GetTraits(#TABLE_NAME);   \
    }                                                                 \
  }                                                                   \



} // end namespace
// --------------------------------------------------------------------

#endif
