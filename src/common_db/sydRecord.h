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

// odb
#include <odb/callback.hxx>
#include <odb/core.hxx>

// --------------------------------------------------------------------
namespace syd {

  // Forward declaration
  class Database;
  class RecordTraitsBase;
  template<class T> class RecordTraits;

  /// Base class for all record (or element, or row) in a table.
  /// Need to be abstract class (pure virtual method)
#pragma db object abstract pointer(std::shared_ptr) callback(Callback)
  class Record:
    public std::enable_shared_from_this<Record> {
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
    friend class syd::RecordTraits<syd::Record>;
    virtual RecordTraitsBase * traits() const = 0;
    virtual std::string GetTableName() const;
    virtual std::string GetSQLTableName() const;
    // ----------------------------------------------------

    // FIXME will be removed
    // static void InitInheritance() { }

    /// Set the values of the fields from some string.
    virtual void Set(const std::vector<std::string> & args);

    /// Use to write the element as a string (must be overloaded)
    virtual std::string ToString() const;

    /// Use to write all fields as a string
    virtual std::string AllFieldsToString() const { return ToString(); }

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

    /// Return the db where this record is stored
    syd::Database * GetDatabase() const;

    /// Return the db where this record is stored and consider it as a DatabaseType
    template<class DatabaseType> DatabaseType * GetDatabase() const;

    /// FIXME
    std::string GetFieldValue(std::string field_name);

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


  /// ---------------------------------
  /// Macros *REQUIRED* in *ALL* tables
  /// ---------------------------------
  /// First macro: in the class definition, public
  /// Second macro: after the class definition in the header
  /// Third macro: in the .cxx file
  ///
  /// friend odb::access is needed for polymorphism
  /// friend RecordTraits access is needed for traits() function

#define DEFINE_TABLE_CLASS(TABLE_NAME)          \
  friend class odb::access;                     \
  friend syd::RecordTraits<TABLE_NAME>;         \
  typedef std::shared_ptr<TABLE_NAME> pointer;  \
  typedef std::vector<pointer> vector;          \
  virtual RecordTraitsBase * traits() const;


#define DEFINE_TABLE_IMPL(TABLE_NAME)               \
  namespace syd {                                   \
    RecordTraitsBase * TABLE_NAME::traits() const { \
      return RecordTraits<TABLE_NAME>::GetTraits(); \
    }                                               \
  }                                                 \

} // end namespace
// --------------------------------------------------------------------

#endif
