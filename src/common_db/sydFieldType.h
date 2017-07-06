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

#ifndef SYDFIELDTYPE_H
#define SYDFIELDTYPE_H

// syd
#include "sydFieldBase.h"
#include "sydCommon.h"

// --------------------------------------------------------------------
namespace syd {

  /*
    This class is the base class for all Field of a given type
  */
  template<class FieldValueType>
    class FieldType: public FieldBase {
  public:

    typedef FieldBase::ToStringFunction ToStringFunction;
    typedef FieldBase::RecordPointer RecordPointer;
    typedef FieldBase::SortFunction SortFunction;
    typedef std::function<FieldValueType & (RecordPointer p)> CastFunction;
    typedef std::function<FieldValueType (RecordPointer p)> ROCastFunction;

    /// Constructor
    FieldType(std::string name);

    /// Destructor
    virtual ~FieldType();

    ToStringFunction BuildToStringFunction(CastFunction f) const;
    ToStringFunction BuildToStringFunction(ROCastFunction f) const;

    ToStringFunction BuildComposedFunction(CastFunction f, ToStringFunction h) const;
    ToStringFunction BuildComposedFunction(ROCastFunction f, ToStringFunction h) const;
    SortFunction BuildComposedFunction(CastFunction f, SortFunction h) const;
    SortFunction BuildComposedFunction(ROCastFunction f, SortFunction h) const;

    SortFunction BuildSortFunction(CastFunction f) const;
    SortFunction BuildSortFunction(ROCastFunction f) const;

  }; // end of class
  // --------------------------------------------------------------------

#define DECLARE_BUILD_TOSTRING_FUNCTION(TYPE)        \
  template<>                                        \
    typename syd::FieldType<TYPE>::ToStringFunction \
    syd::FieldType<TYPE>::                          \
    BuildToStringFunction(CastFunction f) const;    \
  template<>                                        \
    typename syd::FieldType<TYPE>::ToStringFunction \
    syd::FieldType<TYPE>::                          \
    BuildToStringFunction(ROCastFunction f) const;

  /*template<>                                   \
    typename syd::FieldType<TYPE>::SortFunction  \
    syd::FieldType<TYPE>::                          \
    BuildSortFunction(CastFunction f) const;     \
    template<>                                        \
    typename syd::FieldType<TYPE>::SortFunction  \
    syd::FieldType<TYPE>::                          \
    BuildSortFunction(ROCastFunction f) const;
  */

  DECLARE_BUILD_TOSTRING_FUNCTION(std::string);
  DECLARE_BUILD_TOSTRING_FUNCTION(syd::IdType);
  DECLARE_BUILD_TOSTRING_FUNCTION(int);
  DECLARE_BUILD_TOSTRING_FUNCTION(unsigned short int);
  DECLARE_BUILD_TOSTRING_FUNCTION(double);
  //DECLARE_BUILD_TOSTRING_FUNCTION(FieldBase::RecordPointer);
  DECLARE_BUILD_TOSTRING_FUNCTION(bool);

  // I need to declare this function (empty) because the default 'compose' is
  // intended for syd::Record elements.
#define DECLARE_COMPOSE(TYPE)                                           \
  template<> typename syd::FieldType<TYPE>::ToStringFunction            \
    syd::FieldType<TYPE>::                                              \
    BuildComposedFunction(CastFunction f, ToStringFunction h) const;    \
  template<> typename syd::FieldType<TYPE>::ToStringFunction            \
    syd::FieldType<TYPE>::                                              \
    BuildComposedFunction(ROCastFunction f, ToStringFunction h) const;  \
  template<> typename syd::FieldType<TYPE>::SortFunction                \
    syd::FieldType<TYPE>::                                              \
    BuildComposedFunction(CastFunction f, SortFunction h) const;        \
  template<> typename syd::FieldType<TYPE>::SortFunction                \
    syd::FieldType<TYPE>::                                              \
    BuildComposedFunction(ROCastFunction f, SortFunction h) const;

#define DEFINE_COMPOSE(TYPE)                                            \
  template<> typename syd::FieldType<TYPE>::ToStringFunction            \
    syd::FieldType<TYPE>::                                              \
    BuildComposedFunction(CastFunction f, ToStringFunction h)           \
    const { return nullptr;}                                            \
  template<> typename syd::FieldType<TYPE>::ToStringFunction            \
    syd::FieldType<TYPE>::                                              \
    BuildComposedFunction(ROCastFunction f, ToStringFunction h)         \
    const { return nullptr;}                                            \
  template<> typename syd::FieldType<TYPE>::SortFunction                \
    syd::FieldType<TYPE>::                                              \
    BuildComposedFunction(CastFunction f, SortFunction h)               \
    const { DD("BuildComposedFunction sort null"); DD(#TYPE); return nullptr;} \
  template<> typename syd::FieldType<TYPE>::SortFunction                \
    syd::FieldType<TYPE>::                                              \
    BuildComposedFunction(ROCastFunction f, SortFunction h)             \
    const { DD("BuildComposedFunction sort null"); DD(#TYPE); return nullptr;} \

  DECLARE_COMPOSE(syd::IdType);
  DECLARE_COMPOSE(double);
  DECLARE_COMPOSE(std::string);
  DECLARE_COMPOSE(int);
  DECLARE_COMPOSE(unsigned short int);
  DECLARE_COMPOSE(bool);

  // --------------------------------------------------------------------
} // end namespace

#include "sydFieldType.txx"

  // --------------------------------------------------------------------
#endif
