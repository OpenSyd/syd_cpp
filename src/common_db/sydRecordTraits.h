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
    template<class RecordType>
    std::vector<std::shared_ptr<RecordType>> &
    CastRecordVector(const syd::RecordTraitsBase::RecordBaseVector & records);
    template<class RecordType>
    syd::RecordTraitsBase::RecordBaseVector &
    ConvertToRecordBaseVector(const std::vector<std::shared_ptr<RecordType>> & records);
  */

  /*
    This class containts all common elements for a table.
  */
  template<class RecordType>
    class RecordTraits: public syd::RecordTraitsBase {
  public:

    typedef std::shared_ptr<RecordType> pointer;
    typedef std::vector<pointer> vector;
    typedef syd::RecordTraitsBase::RecordBasePointer RecordBasePointer;
    typedef syd::RecordTraitsBase::RecordBaseVector RecordBaseVector;

    // Main static version to get the singleton traits
    static RecordTraitsBase * GetTraits();
    static RecordTraitsBase * GetTraits(std::string table_name);

    // Create a new record associated with a db (it is not inserted yet)
    static pointer New(syd::Database * db);
    virtual RecordBasePointer CreateNew(syd::Database * db) const;

    // Query Insert Update Delete
    virtual RecordBasePointer QueryOne(const syd::Database * db, IdType id) const;
    virtual void Query(const syd::Database * db, RecordBaseVector & r,
                       const std::vector<syd::IdType> & ids) const;
    virtual void Query(const syd::Database * db, RecordBaseVector & r) const;
    virtual void Insert(syd::Database * db, RecordBasePointer record) const;
    virtual void Insert(syd::Database * db, const RecordBaseVector & records) const;
    virtual void Update(syd::Database * db, RecordBasePointer record) const;
    virtual void Update(syd::Database * db, const RecordBaseVector & records) const;
    virtual void Delete(syd::Database * db, const RecordBaseVector & records) const;


    /// other functions
    void Sort(const syd::Database * db, RecordBaseVector & records, const std::string & type) const;
    void Sort(const syd::Database * db, vector & records, const std::string & type) const;

  protected:
    RecordTraits(std::string table_name);
    static RecordTraitsBase * singleton_;

  }; // end of class

  /* template<class RecordType> */
  /*   RecordTraits<RecordType> * GetTrait(std::shared_ptr<RecordType> p); */


#define DEFINE_TABLE_TRAITS_HEADER(TABLE_NAME)  \
  template<> syd::RecordTraitsBase *            \
    RecordTraits<TABLE_NAME>::GetTraits();

#define DEFINE_TABLE_TRAITS_IMPL(TABLE_NAME)                        \
  namespace syd {                                                   \
    template<>                                                      \
      syd::RecordTraitsBase *                                       \
      syd::RecordTraits<TABLE_NAME>::GetTraits() {                  \
      return syd::RecordTraits<TABLE_NAME>::GetTraits(#TABLE_NAME); \
    }                                                               \
  }                                                                 \



  // --------------------------------------------------------------------
} // end namespace

// Must *not* be in namespace syd (because include)
#include "sydRecordTraits.txx"

// --------------------------------------------------------------------

#endif
