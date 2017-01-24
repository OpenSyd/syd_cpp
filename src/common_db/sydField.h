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
    This class is the base class for all Field of a given type in a given table
  */
  template<class RecordType, class FieldValueType>
    class Field: public FieldType<FieldValueType> {
  public:

    typedef FieldBase::pointer pointer;
    typedef FieldBase::GenericFunction GenericFunction;
    typedef FieldBase::RecordPointer RecordPointer;
    typedef std::function<FieldValueType & (RecordPointer p)> CastFunction;
    typedef std::function<FieldValueType & (typename RecordType::pointer p)> Function;

    Function f;

    Field(std::string name, Function f);
    virtual ~Field();
    CastFunction BuildCastFunction(Function f) const;
    pointer CreateField(const syd::Database * db, std::string field_names) const;

  }; // end of class

  // --------------------------------------------------------------------
} // end namespace

#include "sydField.txx"

// --------------------------------------------------------------------
#endif
