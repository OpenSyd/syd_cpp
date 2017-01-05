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

// syd
#include "sydRecordTraitsBase.h"

// --------------------------------------------------------------------
syd::RecordTraitsBase::RecordTraitsBase(std::string table_name)
{
  table_name_ = table_name;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::RecordTraitsBase::GetTableName() const
{
  return table_name_;
}
// --------------------------------------------------------------------
