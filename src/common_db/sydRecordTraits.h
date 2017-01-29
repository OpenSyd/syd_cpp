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
    typedef syd::RecordTraitsBase::FieldMapType FieldMapType;
    typedef std::map<std::string, std::string> FieldFormatMapType;

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
    RecordFieldFunc GetFieldOLD(std::string field) const;
    std::vector<RecordFieldFunc> GetFields(std::string fields) const;
    const FieldFunctionMap & GetFieldMap() const;
    const RecordFieldFunctionMap & GetRecordFieldMap() const;
    virtual std::string GetDefaultFields() const;

    /// Return the list of initial fields
    virtual const FieldMapType & GetFieldsMap(const syd::Database * db) const;
    const FieldFormatMapType & GetFieldFormatsMap(const syd::Database * db) const;

    /// Create and build a field according to the name
    virtual FieldBasePointer NewField(const syd::Database * db, std::string field_name) const;
    virtual FieldBaseVector NewFields(const syd::Database * db, std::string field_names) const;

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

    /// Map of fields (mutable because lazy initialisation)
    mutable FieldMapType field_map_;

    // Map of format string
    mutable FieldFormatMapType field_format_map_;

    /// Initial function to build the fields (will be overloaded)
    void BuildFields(const syd::Database * db, FieldMapType & map) const;

    /// Common fields for all records (id, raw)
    void InitCommonFields(FieldMapType & map) const;

    /// Look in the map to get a field by his name
    FieldBasePointer GetField(const syd::Database * db, std::string field_name) const;

    /// Define a new Field, of a given type by reference
    template<class FieldValueType>
      void AddField(FieldMapType & map,
                    std::string name,
                    std::function<FieldValueType & (typename RecordType::pointer p)> f) const;

    /// Define a new Field, of a given type by value (read only)
    template<class FieldValueType>
      void AddField(FieldMapType & map,
                    std::string name,
                    std::function<FieldValueType (typename RecordType::pointer p)> f) const;

    /// Define a new Field, of a given record pointer
    template<class RecordType2>
      void AddTableField(FieldMapType & map,
                         std::string name,
                         std::function<typename RecordType2::pointer & (typename RecordType::pointer p)> f) const;

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


#define ADD_FIELD(NAME, TYPE)                             \
  {                                                       \
    auto f = [](pointer p) -> TYPE & { return p->NAME; }; \
    AddField<TYPE>(map, #NAME, f);                        \
  }

#define ADD_RO_FIELD(NAME, TYPE)                        \
  {                                                     \
    auto f = [](pointer p) -> TYPE { return p->NAME; }; \
    AddField<TYPE>(map, #NAME, f);                      \
  }

#define ADD_TABLE_FIELD(NAME, TYPE)                                 \
  {                                                                 \
    auto f = [](pointer p) -> TYPE::pointer & { return p->NAME; };  \
    AddTableField<TYPE>(map, #NAME, f);                             \
  }



// Must *not* be in namespace syd (because include)
#include "sydRecordTraits.txx"

// --------------------------------------------------------------------
#endif
