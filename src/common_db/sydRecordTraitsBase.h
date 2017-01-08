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

#ifndef SYDRECORDTRAITSBASE_H
#define SYDRECORDTRAITSBASE_H

// syd
#include "sydCommon.h"

// --------------------------------------------------------------------
namespace syd {

  /*
    This class containts common information (traits) for record.
    We store here information that do not depends on the RecordType.
  */

  class Record;
  class Database;

  class RecordTraitsBase {
  public:

    typedef std::shared_ptr<Record> generic_record_pointer;
    typedef std::vector<generic_record_pointer> generic_record_vector;

    /// Return the table name
    virtual std::string GetTableName() const;

    /// Create a new record
    virtual generic_record_pointer CreateNew(syd::Database * db) const = 0;

    /// Basic functions: Query Insert Update Delete 
    virtual generic_record_pointer QueryOne(const syd::Database * db, syd::IdType id) const = 0;
    virtual void Query(const syd::Database * db,
                       generic_record_vector & r,
                       const std::vector<syd::IdType> & id) const = 0;
    virtual void Query(const syd::Database * db,
                       generic_record_vector & r) const = 0;
    virtual void Insert(syd::Database * db, generic_record_pointer record) const = 0;
    virtual void Insert(syd::Database * db, const generic_record_vector & records) const = 0;
    virtual void Update(syd::Database * db, generic_record_pointer record) const = 0;
    virtual void Update(syd::Database * db, const generic_record_vector & records) const = 0;
    virtual void Delete(syd::Database * db, const generic_record_vector & records) const = 0;

  protected:
    RecordTraitsBase(std::string table_name);
    std::string table_name_;

  }; // end of class
  // --------------------------------------------------------------------

} // end namespace
// --------------------------------------------------------------------

#endif
