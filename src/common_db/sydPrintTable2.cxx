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
#include "sydPrintTable2.h"
//#include "sydRecord.h"
//#include "sydPrintTable.h"
//#include "sydDatabase.h"

//------------------------------------------------------------------
syd::PrintTable2::PrintTable2()
{
  // format_ = "default";
  // use_single_row_flag_ = false;
  // use_header_flag_ = true;
  // AddFormat("raw", "Print every fields as a simple line");
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable2::Build(const RecordBaseVector records, std::string columns)
{
  DDF();
  values_.resize(records.size());
  if (records.size() == 0) return;
  auto table_name = records[0]->GetTableName();
  DD(table_name);
  auto db = records[0]->GetDatabase();
  //auto field = db->GetField(table_name, columns);
  auto fields = db->GetFields(table_name, columns);
  Build(records, fields);
}
//------------------------------------------------------------------


// -----------------------------------------------------------------
// FIXME template Build<T>(T::vector ...)
void syd::PrintTable2::Build(const RecordBaseVector records,
                             const std::vector<FieldFunc> & fields)
{
  DDF();
  values_.resize(records.size());
  int i=0; // row
  for(auto & r:records) {
    //values_[i].resize(1);
    //    values_[i][0] = f(r);
    int j=0; // column
    values_[i].resize(fields.size());
    for(auto & f:fields) {
      values_[i][j] = f(r); // get the value
      ++j;
    }
    ++i;
  }
}
// -----------------------------------------------------------------



// -----------------------------------------------------------------
void syd::PrintTable2::Print(std::ostream & os)
{
  DDF();
  //for(auto & h:headers_) h->Print(os);
  //for(auto & r:rows_) r->Print(os);
  for(auto & row:values_) {
    for(auto & col:row) os << col << " ";
    os << std::endl;
  }
}
// -----------------------------------------------------------------
