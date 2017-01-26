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

// --------------------------------------------------------------------
namespace syd {

  /*
    This class is the base class for all Field of a given type
  */
  template<class FieldValueType>
    class FieldType: public FieldBase {
  public:

    typedef FieldBase::GenericFunction GenericFunction;
    typedef FieldBase::RecordPointer RecordPointer;
    typedef std::function<FieldValueType & (RecordPointer p)> CastFunction;
    typedef std::function<FieldValueType (RecordPointer p)> ROCastFunction;

    FieldType(std::string name);  // FIXME ? protected
    virtual ~FieldType();

    GenericFunction BuildGenericFunction(CastFunction f) const;
    GenericFunction BuildGenericFunction(ROCastFunction f) const;

    //    GenericFunction ComposeGenericFunction(CastFunction f, GenericFunction h) const;
    void Compose(CastFunction f, GenericFunction h);
    void Compose(ROCastFunction f, GenericFunction h);


  }; // end of class
  // --------------------------------------------------------------------

#define DECLARE_BUILD_GENERIC_FUNCTION(TYPE)        \
  template<>                                        \
    typename syd::FieldType<TYPE>::GenericFunction  \
    syd::FieldType<TYPE>::                          \
    BuildGenericFunction(CastFunction f) const;     \
  template<>                                        \
    typename syd::FieldType<TYPE>::GenericFunction  \
    syd::FieldType<TYPE>::                          \
    BuildGenericFunction(ROCastFunction f) const;

  DECLARE_BUILD_GENERIC_FUNCTION(std::string);
  DECLARE_BUILD_GENERIC_FUNCTION(syd::IdType);
  DECLARE_BUILD_GENERIC_FUNCTION(int);
  DECLARE_BUILD_GENERIC_FUNCTION(unsigned short int);
  DECLARE_BUILD_GENERIC_FUNCTION(double);
  DECLARE_BUILD_GENERIC_FUNCTION(FieldBase::RecordPointer);

  // I need to declare this function (empty) because the default 'compose' is
  // intended for syd::Record elements.
#define DECLARE_COMPOSE(TYPE)                     \
  template<> void syd::FieldType<TYPE>::          \
    Compose(CastFunction f, GenericFunction h);   \
  template<> void syd::FieldType<TYPE>::          \
    Compose(ROCastFunction f, GenericFunction h);

#define DEFINE_COMPOSE(TYPE)                        \
  template<> void syd::FieldType<TYPE>::            \
    Compose(CastFunction f, GenericFunction h){}    \
  template<> void syd::FieldType<TYPE>::            \
    Compose(ROCastFunction f, GenericFunction h){}

  DECLARE_COMPOSE(syd::IdType);
  DECLARE_COMPOSE(double);
  DECLARE_COMPOSE(std::string);
  DECLARE_COMPOSE(int);
  DECLARE_COMPOSE(unsigned short int);


  // --------------------------------------------------------------------
} // end namespace

#include "sydFieldType.txx"

// --------------------------------------------------------------------
#endif
