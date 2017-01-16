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
/*void syd::PrintTable2::Build(const RecordBaseVector records, std::string columns)
{
  values_.resize(records.size());
  if (records.size() == 0) return;
  auto table_name = records[0]->GetTableName();
  auto db = records[0]->GetDatabase();
  //auto field = db->GetField(table_name, columns);
  auto fields = db->GetFields(table_name, columns);
  Build(records, fields);
}
*/
//------------------------------------------------------------------


// -----------------------------------------------------------------
// FIXME template Build<T>(T::vector ...)
void syd::PrintTable2::Build(const RecordBaseVector & records,
                             const std::vector<FieldFunc> & fields)
{
  DDF();
  values_.resize(records.size());
  int i=0; // row
  DD(fields.size());
  for(auto & r:records) {
     int j=0; // column
    values_[i].resize(fields.size());
    for(auto & f:fields) {
      values_[i][j] = f(r); // get the value
      //      DD(values_[i][j]);
      ++j;
    }
    ++i;
  }
  column_widths_.resize(fields.size());
  SetDefaultColumnsSize();
}
// -----------------------------------------------------------------



// -----------------------------------------------------------------
void syd::PrintTable2::Print(std::ostream & os)
{
  //for(auto & h:headers_) h->Print(os);
  //for(auto & r:rows_) r->Print(os);
  for(auto & row:values_) {
    PrintRow(os, row);
    os << std::endl;
  }
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
void syd::PrintTable2::SetDefaultColumnsSize()
{
  // compute max length by columns
  std::fill(column_widths_.begin(), column_widths_.end(), 0);
  for(auto & row:values_) {
    int i=0;
    for(auto & col:row) {
      int n = col.size();
      column_widths_[i] = std::max(column_widths_[i], n);
      ++i;
    }
  }
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
void syd::PrintTable2::PrintRow(std::ostream & os, std::vector<std::string> & row)
{
  int i=0;
  for(auto & col:row) {
    os << std::setw(column_widths_[i]);
    os << col << " ";
    ++i;
  }
}
// -----------------------------------------------------------------
