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
#include "sydCommon.h"

// --------------------------------------------------------------------
namespace syd {

  template<class RecordType>
  class RecordTraits {
  public:

    typedef std::function<std::string(syd::Record::pointer)> FieldFunctionType;

    static std::string GetTableName2();
    static const FieldFunctionType & GetFieldFunction2(std::string field_name);

  protected:
    static void BuildFieldFunctionMap();
    static std::string table_name_;
    static std::map<std::string, FieldFunctionType> field_map_;

  }; // end of class
  // --------------------------------------------------------------------

#include "sydRecordTraits.txx"

} // end namespace
// --------------------------------------------------------------------

#endif
