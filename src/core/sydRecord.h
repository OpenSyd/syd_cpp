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

// odb
#include <odb/callback.hxx>
#include <odb/core.hxx>

// --------------------------------------------------------------------
namespace syd {

  class Database;
  struct RecordStat;

  /// Base class for all record (or element, or row) in a table
#pragma db object abstract pointer(std::shared_ptr)
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

    /// Set the values of the fields from some string.
    virtual void Set(const syd::Database * db, const std::vector<std::string> & args);

    /// Initialise a PrintTable according to the format
    virtual void InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const;

    /// Add a line in the given
    virtual void DumpInTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const;

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

  protected:
    /// This default constructor allow to oblige class that inherit
    /// from Record to not have default constructor
    Record();

    /// Copy all fields form the given record (protected to avoid use
    /// with generic pointer)
    virtual void CopyFrom(const pointer p);

    /// Return true if the record is equal (same id here); (protected
    /// to avoid use with generic pointer)
    virtual bool IsEqual(const pointer p) const;


  }; // end of class

    /// odb::access is needed for polymorphism
#define TABLE_DEFINE(TABLE_NAME)                                        \
  typedef std::shared_ptr<TABLE_NAME> pointer;                          \
  typedef std::vector<pointer> vector;                                  \
  friend class odb::access;                                             \
  virtual std::string GetTableName() const { return #TABLE_NAME; }      \
  static std::string GetStaticTableName() { return #TABLE_NAME; }       \
  static pointer New() { return pointer(new TABLE_NAME); }              \

#define TABLE_DECLARE_MANDATORY_FUNCTIONS(TABLE_NAME)           \
  virtual std::string ToString() const;                         \
  virtual bool IsEqual(const pointer p) const;                  \
  virtual void CopyFrom(const pointer p);                       \

#define TABLE_DECLARE_OPTIONAL_FUNCTIONS(TABLE_NAME)                    \
  virtual void Set(const syd::Database * db, const std::vector<std::string> & args); \
  virtual void InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const; \
  virtual void DumpInTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const;


} // end namespace
  // --------------------------------------------------------------------

#endif
