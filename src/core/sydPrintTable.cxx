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
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::AddColumn(std::string name, int w, int digit, bool trunc_by_end_flag)
{
  PrintColumn c;
  columns_.push_back(c);
  auto & col = columns_.back();
  col.title = name;
  col.width = w;
  col.index = columns_.size()-1; // last

  map_column[name]=col.index; // index

  for(auto row:rows_) {
    if (row.values.size() != columns_.size()) row.values.resize(columns_.size());
  }

  // if (w == -1) w = name.size()+1;
  // headers.push_back(name);
  // width.push_back(w);
  // precision.push_back(digit);
  // // Later
  // if (current_line != -1) values[current_line].resize(headers.size());
  // trunc_by_end.push_back(trunc_by_end_flag);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::SetColumnWidth(int col, int w)
{
  width[col] = w;
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
void syd::PrintTable::Endl()
{
  std::vector<std::string> line(headers.size());
  values.push_back(line);
  current_line++;
  current_column = 0;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
syd::PrintTable & syd::PrintTable::operator<<(const double & value)
{
  if (current_line == -1) Endl();
  std::stringstream ss;
  if (fabs(value) < 1e-4 and value != 0.0)
    ss << std::scientific << std::setprecision (precision[current_column]) << value;
  else
    ss << std::fixed << std::setprecision (precision[current_column]) << value;
  values[current_line][current_column] = ss.str();
  current_column++;
  return *this;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
syd::PrintTable & syd::PrintTable::operator<<(const std::string & value)
{
  if (current_line == -1) Endl();
  // Trunc string if too big
  std::string v = value;
  if (v.size() >= width[current_column]) {
    if (trunc_by_end[current_column]) v = value.substr(0,width[current_column]-4)+"...";
    else v = "..."+value.substr(v.size()-width[current_column]+4,v.size());
  }
  values[current_line][current_column] = v;
  current_column++;
  return *this;
}
//------------------------------------------------------------------


void syd::PrintTable::Set(int col, const std::string & value)
{
  // DD("Set");
  // DD(col);
  // DD(value);
  auto & row = rows_.back(); // last one is current one
  // DDS(row.values);
  // DD(row.values.size());
  if (row.values.size() != columns_.size()) row.values.resize(columns_.size());
  row.values[col] = value;
}

void syd::PrintTable::Set(const std::string & col_name, const std::string & value)
{
  // DD(map_column[col_name]);
  // DD(value);
  if (map_column.find(col_name) == map_column.end()) { // FIXME -> put function
    DD("not find");
    EXCEPTION("Error cannot set column named '" << col_name << "'.");
  }
  else Set(map_column[col_name], value);
}

void syd::PrintTable::Set(const std::string & col_name, const double & value)
{
  // DD(map_column[col_name]);
  // DD(value);
  Set(map_column[col_name], value);
}

void syd::PrintTable::Set(int col, const double & value)
{
  //DD("set double");
  //DD(value);
  std::stringstream ss;
  //  ss << std::fixed << std::scientific << std::setprecision(0) << value; //FIXME
  ss << std::fixed << std::setprecision(0) << value; //FIXME
  Set(col, ss.str());
}


//------------------------------------------------------------------
void syd::PrintTable::SkipLine()
{
  for(auto i=0; i<headers.size(); i++) *this << " ";
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::Print(std::ostream & out)
{
  // Compute optimal column width
  for(auto & col:columns_) {
    unsigned long m = 0;//col.width; // requested min and or max? FIXME
    if (col.width != 0) {
      for(auto row:rows_) {
        if (col.index < row.values.size())
          m = std::max(m, row.values[col.index].size());
        //    if (m > max width change ....
      }
      col.width = m+1; // FIXME spacing between col
    }
  }

  // Dump headers
  for(auto col:columns_) { // FIXME order
    if (col.width != 0) out << std::setw(col.width) << col.title;
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


void syd::PrintTable::DumpRow(const syd::PrintRow & row, std::ostream & out)
{
  for(auto col:columns_) {
    std::string s="-"; // default output if column not known
    if (col.index < row.values.size()) s=row.values[col.index];
    out << std::setw(col.width) << s;
  }
}


bool syd::PrintTable::ColumnsAreDefined(const std::string & table_name) {
  // FIXME depend on format
  if (map_column_defined.find(table_name) != map_column_defined.end())
    return map_column_defined[table_name];
  else return false;
}


void syd::PrintTable::SetColumnsAreDefined(const std::string & table_name)
{
  map_column_defined[table_name] = true;
}
