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

    FieldType(std::string name);  // FIXME ? protected
    virtual ~FieldType();

    GenericFunction BuildGenericFunction(CastFunction f) const;

    //    GenericFunction ComposeGenericFunction(CastFunction f, GenericFunction h) const;
    void Compose(CastFunction f, GenericFunction h);


  }; // end of class
  // --------------------------------------------------------------------

  template<>
    typename syd::FieldType<std::string>::GenericFunction
    syd::FieldType<std::string>::BuildGenericFunction(CastFunction f) const;

  template<>
    typename syd::FieldType<syd::IdType>::GenericFunction
    syd::FieldType<syd::IdType>::BuildGenericFunction(CastFunction f) const;

  template<>
    typename syd::FieldType<double>::GenericFunction
    syd::FieldType<double>::BuildGenericFunction(CastFunction f) const;

  template<>
    typename syd::FieldType<FieldBase::RecordPointer>::GenericFunction
    syd::FieldType<FieldBase::RecordPointer>::BuildGenericFunction(CastFunction f) const;

  // I need to declare this function (empty) because the default 'compose' is
  // intended for syd::Record elements.
  template<> void syd::FieldType<syd::IdType>::Compose(CastFunction f, GenericFunction h);
  template<> void syd::FieldType<std::string>::Compose(CastFunction f, GenericFunction h);
  template<> void syd::FieldType<double>::Compose(CastFunction f, GenericFunction h);

  // --------------------------------------------------------------------
} // end namespace

#include "sydFieldType.txx"

// --------------------------------------------------------------------
#endif
