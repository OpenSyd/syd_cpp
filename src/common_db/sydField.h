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

#ifndef SYDFIELD_H
#define SYDFIELD_H

// syd
#include "sydFieldType.h"

// --------------------------------------------------------------------
namespace syd {

  /*
    This class manage a field of a table. The type of the table is 'RecordType'
    while the type of the field is 'FieldValueType'.
    The function 'f' allows to retrieve the reference to a given record.
  */
  template<class RecordType, class FieldValueType>
    class Field: public FieldType<FieldValueType> {
  public:

    // Typedefs
    typedef Field<RecordType,FieldValueType> Self;
    typedef std::shared_ptr<Self> SelfPointer;
    typedef FieldBase::pointer pointer;
    typedef FieldBase::ToStringFunction ToStringFunction;
    typedef FieldBase::SortFunction SortFunction;
    typedef FieldBase::RecordPointer RecordPointer;
    typedef std::function<FieldValueType & (RecordPointer p)> CastFunction; // FIXME from FieldType
    typedef std::function<FieldValueType & (typename RecordType::pointer p)> Function;
    typedef std::function<FieldValueType (RecordPointer p)> ROCastFunction;
    typedef std::function<FieldValueType (typename RecordType::pointer p)> ROFunction;

    Function f;
    ROFunction rof;

    /// Constructor (function by reference)
    Field(std::string name, Function f);

    /// Constructor (function by value)
    Field(std::string name, ROFunction f);

    /// Destructor
    virtual ~Field();

    /// Main function. Must be call before using 'get' or 'set'
    virtual void BuildFunction(const syd::Database * db);

    /// Change the precision. Will rebuild the function
    virtual void SetPrecision(int p);

    /// Create a new pointer (function with ref)
    static SelfPointer New(std::string name, Function f, bool read_only, std::string abbrev);

    /// Create a new pointer (function wo ref)
    static SelfPointer New(std::string name, ROFunction f, bool read_only, std::string abbrev);

    /// Return a copy
    virtual pointer Copy() const;

    virtual std::string ToString() const;

    /// Default function to print an element (must be inline here).
    friend std::ostream& operator<<(std::ostream& os, const Self & p) {
      os << p.ToString();
      return os;
    }
    friend std::ostream& operator<<(std::ostream& os, const std::shared_ptr<Self> p) {
      os << p->ToString();
      return os;
    }

  protected:
    CastFunction BuildCastFunction(Function f) const;
    ROCastFunction BuildCastFunction(ROFunction f) const;
    //    pointer CreateField(const syd::Database * db, std::string field_names) const;

    /* static pointer CreateField(std::string name, Function f, std::string type=""); */
    /* static pointer CreateField(std::string name, ROFunction f, std::string type=""); */

  }; // end of class


  /// Build a cast function with a return by reference
  /* template<class RecordType, class FieldValueType> */
  /*   std::function<FieldValueType & (typename FieldBase::RecordPointer)> */
  /*   BuildCastRefFunction(std::function<FieldValueType & (typename RecordType::pointer)> f); */

  /* /// Build a cast function with a return by value */
  /* template<class RecordType, class FieldValueType> */
  /*   std::function<FieldValueType (typename FieldBase::RecordPointer)> */
  /*   BuildCastValueFunction(std::function<FieldValueType (typename RecordType::pointer)> f); */

  // --------------------------------------------------------------------
} // end namespace

#include "sydField.txx"

// --------------------------------------------------------------------
#endif
