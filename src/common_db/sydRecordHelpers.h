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
#include "sydRecordWithTags.h"

// --------------------------------------------------------------------
namespace syd {

  template<class RecordType>
  typename RecordType::vector BindTo(const syd::Record::vector & records);
  template<class RecordType>
  typename syd::Record::vector BindFrom(const typename RecordType::vector & records);

  template<class RecordType>
  typename RecordType::vector
  KeepRecordIfContainsAllTags(const typename RecordType::vector & records,
                              const std::vector<std::string> & tag_names);

  template<class RecordType>
  typename RecordType::vector
  KeepRecordIfContainsAllTags(const typename RecordType::vector & records,
                              const std::string & tag_name);

  bool ContainsAllTags(const syd::RecordWithTags::pointer & record,
                       const std::vector<std::string> & tag_names);


}

#include "sydRecordHelpers.txx"
// --------------------------------------------------------------------

#endif
