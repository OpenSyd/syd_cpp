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
#include "sydRecord.h"
#include "sydDatabase.h"

//------------------------------------------------------------------
syd::PrintTable::PrintTable()
{
  header_flag_ = true;
  footer_flag_ = true;
  precision_ = -1;
  single_line_flag_ = false;
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
void syd::PrintTable::SetSingleLineFlag(bool b)
{
  single_line_flag_ = b;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::Build(std::string table_name,
                            const RecordBaseVector records,
                            std::string format)
{
  values_.resize(records.size());
  if (records.size() == 0) return;
  auto db = records[0]->GetDatabase();
  auto fields = db->GetFields(table_name, format);
  Build(records, fields);
}
//------------------------------------------------------------------


// -----------------------------------------------------------------
void syd::PrintTable::Build(const RecordBaseVector & records,
                            syd::FieldBase::vector & fields)
{
  values_.resize(records.size());
  if (records.size() == 0) return;

  // Header
  header_.resize(fields.size());
  int i=0;
  for(auto f:fields) {
    header_[i] = f->abbrev;
    ++i;
  }

  // Precision
  if (precision_ != -1) {
    auto db = records[0]->GetDatabase();
    for(auto &f:fields) {
      f->precision = precision_;
      f->BuildFunction(db);
    }
  }

  i=0; // row
  for(auto & r:records) {
    int j=0; // column
    values_[i].resize(fields.size());
    for(auto & f:fields) {
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
    if (!single_line_flag_) os << std::endl;
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
