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

  template<class RecordType>
    std::vector<std::shared_ptr<RecordType>>
    CastFromVectorOfRecords(const syd::RecordTraitsBase::RecordBaseVector & records);
  template<class RecordType>
    syd::RecordTraitsBase::RecordBaseVector
    ConvertToVectorOfRecords(const std::vector<std::shared_ptr<RecordType>> & records);

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
    typedef std::function<bool(pointer a, pointer b)> CompareFunction;
    typedef std::map<std::string, CompareFunction> CompareFunctionMap;
    typedef std::function<std::string(pointer)> FieldFunc;
    typedef std::map<std::string, FieldFunc> FieldFunctionMap;
    typedef syd::RecordTraitsBase::RecordFieldFunctionMap RecordFieldFunctionMap;
    //typedef syd::RecordTraitsBase::FieldBasePointer FieldBasePointer;

    // Main static version to get the singleton traits
    static RecordTraits<RecordType> * GetTraits();

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

    /// Function to sort elements in a vector
    void Sort(RecordBaseVector & records, const std::string & type) const;
    const CompareFunctionMap & GetSortFunctionMap() const;
    void BuildMapOfSortFunctions(CompareFunctionMap & map) const;

    /// Functions to get fields value as string
    void BuildMapOfFieldsFunctions(FieldFunctionMap & map) const;
    RecordFieldFunc GetField(std::string field) const;
    std::vector<RecordFieldFunc> GetFields(std::string fields) const;
    const FieldFunctionMap & GetFieldMap() const;
    const RecordFieldFunctionMap & GetRecordFieldMap() const;
    virtual std::string GetDefaultFields() const;

    /// FIXME
    typedef std::map<std::string, FieldBasePointer> FieldMapType;
    const FieldMapType & GetFieldMap2() const;
    void BuildFields(FieldMapType & map) const;
    void InitCommonFields(FieldMapType & map) const;
    FieldBasePointer GetField2(const syd::Database * db, std::string field_name) const;
    FieldBasePointer GetField2(std::string field_name) const; // FIXME change name 

  protected:
    RecordTraits(std::string table_name);

    /// Really build the singleton
    static RecordTraits<RecordType> * GetTraits(std::string table_name);

    /// Unique instance (singleton)
    static RecordTraits<RecordType> * singleton_;

    // For sorting elements. The following is mutable because may be
    // initialized the first time it is call (from a const function)
    void InternalSort(vector & records, const std::string & type) const;
    mutable CompareFunctionMap compare_record_fmap_;
    void SetDefaultSortFunctions(CompareFunctionMap & map) const;

    // For get field function
    mutable FieldFunctionMap field_fmap_;
    void SetDefaultFieldFunctions(FieldFunctionMap & map) const;
    void InitFields() const;

    /// FIXME
    mutable FieldMapType field_map_;

  }; // end of class

  /// Macros to simplify the traits classes
#define DEFINE_TABLE_TRAITS_HEADER(TABLE_NAME)  \
  template<> syd::RecordTraits<TABLE_NAME> *    \
    RecordTraits<TABLE_NAME>::GetTraits();

#define DEFINE_TABLE_TRAITS_IMPL(TABLE_NAME)                        \
  namespace syd {                                                   \
    template<>                                                      \
      syd::RecordTraits<TABLE_NAME> *                               \
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
