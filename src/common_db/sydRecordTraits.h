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

#ifndef SYDRECORDTRAITS_H
#define SYDRECORDTRAITS_H

// syd
#include "sydRecordTraitsBase.h"

// --------------------------------------------------------------------
namespace syd {

  /*
    This class containts all common elements for a table.
    - GetTableName
    - TODO GetSQLTableName
    - TODO Fields
    - TODO insert et al
  */

  template<class RecordType>
    class RecordTraits: public syd::RecordTraitsBase {
  public:

    typedef std::shared_ptr<RecordType> pointer;
    typedef std::vector<pointer> vector;
    typedef syd::RecordTraitsBase::generic_record_pointer generic_record_pointer;

    // Main static version to get the singleton traits
    static RecordTraitsBase * GetTraits();
    static RecordTraitsBase * GetTraits(std::string table_name);

    // Create a new record associated with a db (it is not inserted yet)
    static pointer New(syd::Database * db);
    virtual generic_record_pointer CreateNew(syd::Database * db) const;

  protected:
    RecordTraits(std::string table_name);
    static RecordTraitsBase * singleton_;

  }; // end of class
  // --------------------------------------------------------------------

#include "sydRecordTraits.txx"

} // end namespace
// --------------------------------------------------------------------

#endif
