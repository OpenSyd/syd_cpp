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
#include "sydRecord.h"

// std
#include <algorithm>
#include <sstream>

//------------------------------------------------------------------
syd::PrintTable2::PrintTable2()
{
  format_ = "default";
  use_single_row_flag_ = false;
  use_header_flag_ = true;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable2::Build(syd::Record::vector::const_iterator start,
                             syd::Record::vector::const_iterator end)
{
  if (start == end) return;

  // Create the table
  int nb_of_rows = end-start;
  for(auto i=start; i != end; i++) {
    auto row = syd::PrintTableRow::New(this);
    current_row_ = row;
    (*i)->DumpInTable(*this);
    rows_.push_back(row);
    table_names_.insert((*i)->GetTableName());
  }
  auto indices = GetColumnsIndices();

  // Set user precision
  // FIXME

  // Compute optimal column width
  for(auto i:indices)
    for(auto & r:rows_) {
      columns_[i]->UpdateWidth(r->GetValue(i));
    }
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable2::Print(std::ostream & os)
{
  auto indices = GetColumnsIndices();
  int nb_of_rows = rows_.size();

  // If no columns --> help
  if (indices.size() == 0) {
    os << "Available formats are:" << std::endl;
    for(auto f:formats_) {
      os << "\t" << f.name_ << " -> " << f.description_ << std::endl;
    }
    return;
  }

  // Print header
  if (use_header_flag_) {
    os << "\e[1m" << columns_[0]->GetHeaderColor() // bold
       << "# Find " << nb_of_rows
       << " elements in table: ";
    for(auto t:table_names_) os << t << " ";
    os << std::endl;
    for(auto i:indices)
      columns_[i]->DumpHeader(os);
    os << resetColor << std::endl;
  }

  // Print the created table
  for(auto & r:rows_)
    r->Dump(indices, os);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable2::AddFormat(std::string name, std::string description)
{
  FormatType f;
  f.name_ = name;
  f.description_ = description;
  formats_.insert(f);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable2::Set(std::string column_name, std::string value, int width_max)
{
  auto col = GetColumnInfo(column_name);
  col->SetMaxWidth(width_max);
  Set(col, value);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable2::Set(syd::PrintTableColumnInfo::pointer column,
                           std::string value)
{
  current_row_->Set(column->GetIndex(), value);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable2::Set(std::string column_name, double value, int precision)
{
  auto column = GetColumnInfo(column_name);
  Set(column, column->GetStringValue(value, precision));
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable2::SetColumnPrecision(int col, int precision)
{
  columns_[col]->SetPrecision(precision);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
syd::PrintTableColumnInfo::pointer
syd::PrintTable2::GetColumnInfo(int col)
{
  return columns_[col];
}
//------------------------------------------------------------------


//------------------------------------------------------------------
syd::PrintTableColumnInfo::pointer
syd::PrintTable2::GetColumnInfo(std::string column_name)
{
  auto i = columns_name_to_indices_.find(column_name);
  if (i == columns_name_to_indices_.end()) {
    // not exist -> create a new column
    auto column = syd::PrintTableColumnInfo::New(columns_.size());
    column->SetName(column_name);
    columns_name_to_indices_[column_name] = column->GetIndex();
    columns_.push_back(column);
    return column;
  }
  else return columns_[i->second];
}
//------------------------------------------------------------------


//------------------------------------------------------------------
std::string syd::PrintTable2::GetFormat() const
{
  return format_;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable2::SetFormat(std::string f)
{
  if (f == "") format_ = "default";
  else format_ = f;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable2::SetSingleRowFlag(bool b)
{
  use_single_row_flag_ = b;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
std::vector<int> syd::PrintTable2::GetColumnsIndices() const
{
  std::vector<int> c;
  for(auto col:columns_) c.push_back(col->GetIndex());
  return c;
}
//------------------------------------------------------------------
