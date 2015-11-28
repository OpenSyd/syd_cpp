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

#include "sydPrintTable.h"
#include <algorithm>

//------------------------------------------------------------------
syd::PrintTable::PrintTable()
{
  Init();
  current_format_name_ = "default";
}
//------------------------------------------------------------------


//------------------------------------------------------------------
syd::PrintColumn & syd::PrintTable::AddColumn(std::string name, int precision)
{
  if (GetColumn(name) != -1) {
    LOG(FATAL) << "Error redefined column " << name;
  }

  // Create new column
  PrintColumn c;
  columns_.push_back(c);
  auto & col = columns_.back();
  col.title = name;
  col.index = columns_.size()-1; // last
  col.precision = precision;
  col.width = name.size(); // default, will be changed later

  // Default index
  map_column[name]=col.index; // index

  // Update already existing rows
  for(auto row:rows_) {
    if (row.values.size() != columns_.size()) row.values.resize(columns_.size());
  }
  // return ref
  return columns_.back();
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::Init()
{
  current_line = -1;
  current_column = 0;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::Set(int col, const std::string & value)
{
  auto & row = rows_.back(); // last one is current one
  if (row.values.size() != columns_.size()) row.values.resize(columns_.size());
  row.values[col] = value;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::Set(const std::string & col_name, const std::string & value)
{
  if (GetColumn(col_name) == -1) {
    EXCEPTION("Error cannot set column named '" << col_name << "'.");
  }
  Set(map_column[col_name], value);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::Set(const std::string & col_name, const double & value)
{
  if (GetColumn(col_name) == -1) {
    EXCEPTION("Error cannot set column named '" << col_name << "'.");
  }
  Set(map_column[col_name], value);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::Set(int col, const double & value)
{
  std::stringstream ss;
  //  ss << std::fixed << std::scientific << std::setprecision(0) << value; //FIXME
  ss << std::fixed << std::setprecision(columns_[col].precision) << value; //FIXME
  Set(col, ss.str());
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::Print(std::ostream & out)
{
  // Compute optimal column width
  for(auto & col:columns_) {
    unsigned long m = col.title.size();
    if (col.width != 0) {
      for(auto row:rows_) {
        if (col.index < row.values.size())
          m = std::max(m, row.values[col.index].size());
        //    if (m > max width change ....
      }
      col.width = m+1; // FIXME spacing between col
      col.width = std::min(col.width, col.max_width);
    }
  }

  // Trunc if needed
  for(auto & row:rows_) {
    for(int c=0; c<row.values.size(); c++) {
      auto & col = columns_[c];
      std::string v = row.values[c];
      if (v.size() > col.max_width) {
        if (col.trunc_by_end_flag) v = v.substr(0,col.max_width-4)+"...";
        else v = "..."+v.substr(v.size()-col.max_width+4,v.size());
      }
      row.values[c] = v;
    }
  }

  // Dump headers
  out << "#Table: " << current_table_ << ". Number of rows: " << rows_.size() << std::endl;
  bool first = true;
  for(auto col:columns_) { // FIXME order
    if (col.width != 0) {
      if (first) { // special case for first column, start with #
        out << "#" << std::setw(col.width-1) << col.title;
        first = false;
      }
      else out << std::setw(col.width) << col.title;
    }
  }
  out << std::endl;

  // Dump values
  for(auto & row:rows_) {
    DumpRow(row, out);
    out << std::endl;
  }
  out << std::flush;

}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::DumpRow(const syd::PrintRow & row, std::ostream & out)
{
  for(auto col:columns_) {
    std::string s="-"; // default output if column not known
    if (col.index < row.values.size()) s=row.values[col.index];
    out << std::setw(col.width) << s;
  }
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::AddFormat(std::string name, std::string help)
{
  syd::PrintFormat f;
  f.name = name;
  f.help = help;
  formats_.push_back(f);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
int syd::PrintTable::GetColumn(std::string col)
{
  auto iter = std::find_if(columns_.begin(), columns_.end(),
                           [&col](const syd::PrintColumn & c) { return c.title == col; });
  if (iter == columns_.end()) return -1;
  return iter-columns_.begin();
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::SetFormat(std::string name)
{
  current_format_name_ = name;
  if (name == "") current_format_name_ = "default";
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::AddRow()
{
  syd::PrintRow ro;
  rows_.push_back(ro);
  auto & row = rows_.back();
  row.values.resize(columns_.size());
  // initialize
  for(auto & v:row.values) v="-"; // Set to empty values
}
//------------------------------------------------------------------
