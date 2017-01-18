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
  header_flag_ = true;
  footer_flag_ = true;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable2::Build(std::string table_name, const RecordBaseVector records, std::string columns)
{
  values_.resize(records.size());
  if (records.size() == 0) return;
  auto db = records[0]->GetDatabase();

  auto fields_names = columns;
  if (fields_names == "") fields_names = db->GetTraits(table_name)->GetDefaultFields();
  DD(fields_names);
  auto fields = db->GetFields(table_name, fields_names);
  DD(fields.size());
  // Header
  header_.resize(fields.size());
  std::vector<std::string> words;
  syd::GetWords(words, fields_names);
  DD(words.size());
  header_.resize(words.size());
  int i=0;
  if (words.size() != fields.size()) {
    LOG(FATAL) << "Internal error fields in syd::PrintTable2::Build";
  }
  for(auto col:words) {
    header_[i] = col;
    ++i;
  }
  DDS(header_);
  // Build
  Build(records, fields);
}
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
  // Columns width
  column_widths_.resize(fields.size());
  SetDefaultColumnsSize();
}
// -----------------------------------------------------------------



// -----------------------------------------------------------------
void syd::PrintTable2::Print(std::ostream & os)
{
  //for(auto & h:headers_) h->Print(os);
  //for(auto & r:rows_) r->Print(os);
  if (header_flag_) PrintHeader(os);
  for(auto & row:values_) {
    PrintRow(os, row);
    os << std::endl;
  }
  if (footer_flag_) PrintHeader(os);
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
      int n = col.size()+1;
      column_widths_[i] = std::max(column_widths_[i], n);
      ++i;
    }
  }
  int i=0;
  for(auto h:header_) {
    int n = h.size()+1;
    column_widths_[i] = std::max(column_widths_[i], n);
    ++i;
  }
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
void syd::PrintTable2::PrintRow(std::ostream & os, std::vector<std::string> & row)
{
  int i=0;
  for(auto & col:row) {
    os << std::setw(column_widths_[i]) << col;
    ++i;
  }
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
void syd::PrintTable2::PrintHeader(std::ostream & os)
{
  int i=0;
  DDS(column_widths_);
  for(auto & h:header_) {
    os << std::setw(column_widths_[i]) << h;
    ++i;
  }
  os << std::endl;
}
// -----------------------------------------------------------------
