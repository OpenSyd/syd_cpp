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

#ifndef SYDRECORDHELPERS_H
#define SYDRECORDHELPERS_H

// syd
#include "sydRecord.h"

// --------------------------------------------------------------------
namespace syd {


  /// CANNOT MANAGE TO MAKE THIS WORK ...


  /// Cast generic records to RecordType
  template<class RecordType>
  typename RecordType::vector BindTo(const syd::Record::vector & records);

  /// Consider RecordType as generic records
  template<class RecordType>
  typename syd::Record::vector BindFrom(const typename RecordType::vector & records);

}
#include "sydRecordHelper.txx"
// --------------------------------------------------------------------

#endif
