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

#include "sydDatabase.h"


// --------------------------------------------------------------------
template<class Record>
std::shared_ptr<syd::Record> syd::Table<Record>::New() const
{
  DD("Table<Record>::New()");
  return Record::New();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class Record>
void syd::Table<Record>::Insert2(std::shared_ptr<syd::Record> record) const
{
  //  auto p = std::dynamic_pointer_cast<Record>(record);
  auto p = std::static_pointer_cast<Record>(record);
  db_->Insert<Record>(p);
}
// --------------------------------------------------------------------
