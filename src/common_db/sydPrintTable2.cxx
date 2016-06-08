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
  //header_flag_ = true;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable2::Dump(syd::Record::vector::const_iterator start,
                            syd::Record::vector::const_iterator end)
{
  if (start == end) return;

  // Create the table
  int nb_of_rows = end-start;
  for(auto i=start; i != end; i++) {
    auto row = syd::PrintTableRow::New(this);
    current_row_ = row;
    (*i)->DumpInTable(*this); // FIXME format
    rows_.push_back(row);
  }
  auto indices = GetColumnsIndices();
  DDS(indices);

  // Add empty columns, values

  // Set user precision

  // Compute optimal column width
  for(auto i:indices)
    for(auto & r:rows_) {
      columns[i]->UpdateWidth(r->GetValue(i));
    }

  DD("end");
  // Print the created table
  std::ostringstream os;
  for(auto & r:rows_)
    r->Dump(indices, os);
  std::cout << os.str();
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable2::Set(std::string column_name, std::string value)
{
  auto col = GetColumnInfo(column_name);
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
syd::PrintTableColumnInfo::pointer
syd::PrintTable2::GetColumnInfo(int col)
{
  return columns[col];
}
//------------------------------------------------------------------


//------------------------------------------------------------------
syd::PrintTableColumnInfo::pointer
syd::PrintTable2::GetColumnInfo(std::string column_name)
{
  auto i = columns_name_to_indices.find(column_name);
  if (i == columns_name_to_indices.end()) {
    // not exist -> create a new column
    auto column = syd::PrintTableColumnInfo::New(columns.size());
    column->SetName(column_name);
    columns_name_to_indices[column_name] = column->GetIndex();
    columns.push_back(column);
    return column;
  }
  else return columns[i->second];
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
std::vector<int> syd::PrintTable2::GetColumnsIndices()
{
  std::vector<int> c;
  for(auto col:columns) c.push_back(col->GetIndex());
  return c;
}
//------------------------------------------------------------------
