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
#include "sydTableOfRecords.h"
#include "sydDatabase.h"

// --------------------------------------------------------------------
syd::TableOfRecords::TableOfRecords()
{
  DDF();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TableOfRecords::Set(syd::Record::vector r)
{
  DDF();
  records = r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TableOfRecords::AddField(std::string field_name)
{
  DDF();
  if (records.size() == 0) {
    LOG(FATAL) << "Must use Set(records) before";
  }
  auto start = *records.begin();
  auto table_name = start->GetTableName();
  auto db = start->GetDatabase();
  auto f = db->FieldGetter(table_name, field_name);
  fields.push_back(f);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TableOfRecords::Print(std::ostream & os) const
{
  DDF();
  //  if (start == end) return;
  //fields = db->GetFields(start->GetTableName());
  // or
  //fields.push_back(start->field_getter("pname"));
  //  fields.push_back(start->field_getter("study_id"));

  for(auto & r:records)
    Print(os, r);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::TableOfRecords::Print(std::ostream & os, syd::Record::pointer record) const
{
  for(auto &get_field:fields) {
    auto s = get_field(record);
    std::cout << s << " ";
  }
  std::cout << std::endl;
}
// --------------------------------------------------------------------
