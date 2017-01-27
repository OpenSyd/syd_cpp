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
#include "sydPrintTable.h"

//------------------------------------------------------------------
syd::PrintTable::PrintTable()
{
  header_flag_ = true;
  footer_flag_ = true;
  precision_ = -1;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::SetHeaderFlag(bool b)
{
  header_flag_ = b;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::SetFooterFlag(bool b)
{
  footer_flag_ = b;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::SetPrecision(int p)
{
  precision_ = p;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::Build(std::string table_name,
                            const RecordBaseVector records,
                            std::string columns)
{
  values_.resize(records.size());
  if (records.size() == 0) return;
  auto db = records[0]->GetDatabase();

  auto fields_names = columns;

  /*
    if (fields_names == "") fields_names = db->GetTraits(table_name)->GetDefaultFields();
    auto fields = db->GetFields(table_name, fields_names);
  */
  std::vector<std::string> field_names;
  syd::GetWords(field_names, columns);
  DDS(field_names);
  std::vector<syd::FieldBase::pointer> fields;
  for(auto f:field_names) fields.push_back(db->NewField(table_name, f));

  //  auto fields = db->GetFields2(table_name, f);


  // Header
  header_.resize(fields.size());
  // std::vector<std::string> words;
  // syd::GetWords(words, fields_names);
  header_.resize(field_names.size());
  int i=0;
  // if (words.size() != fields.size()) {
  //   LOG(FATAL) << "Internal error fields in syd::PrintTable::Build";
  // }
  /*
    for(auto col:words) {
    header_[i] = col;
    ++i;
    }*/
  // Build
  Build(records, fields);
}
//------------------------------------------------------------------


// -----------------------------------------------------------------
void syd::PrintTable::Build(const RecordBaseVector & records,
                            syd::FieldBase::vector & fields)
{
  values_.resize(records.size());
  if (records.size() == 0) return;

  DD(precision_);
  if (precision_ != -1) {
    DD(precision_);
    auto db = records[0]->GetDatabase();
    for(auto &f:fields) f->precision = precision_;
    for(auto &f:fields) DD(f->precision);
    for(auto &f:fields) DD(f->name);
    for(auto &f:fields) f->BuildFunction(db, "");
  }

  int i=0; // row
  for(auto & r:records) {
    int j=0; // column
    values_[i].resize(fields.size());
    for(auto & f:fields) {
      DD(f->precision);
      values_[i][j] = f->get(r); // get the value
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
void syd::PrintTable::Print(std::ostream & os)
{
  if (header_flag_) PrintHeader(os);
  for(auto & row:values_) {
    PrintRow(os, row);
    os << std::endl;
  }
  if (footer_flag_) PrintHeader(os);
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
void syd::PrintTable::SetDefaultColumnsSize()
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
void syd::PrintTable::PrintRow(std::ostream & os, std::vector<std::string> & row)
{
  int i=0;
  for(auto & col:row) {
    os << std::setw(column_widths_[i]) << col;
    ++i;
  }
}
// -----------------------------------------------------------------


// -----------------------------------------------------------------
void syd::PrintTable::PrintHeader(std::ostream & os)
{
  int i=0;
  for(auto & h:header_) {
    os << std::setw(column_widths_[i]) << h;
    ++i;
  }
  os << std::endl;
}
// -----------------------------------------------------------------
