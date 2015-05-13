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

//------------------------------------------------------------------
syd::PrintTable::PrintTable()
{
  Init();
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::AddColumn(std::string name, int w, int digit)
{
  if (w == -1) w = name.size()+1;
  headers.push_back(name);
  width.push_back(w);
  precision.push_back(digit);
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::Init()
{
  current_line = 0;
  current_column = 0;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
syd::PrintTable & syd::PrintTable::operator<<(const double & value)
{
  if (values.size() == current_line) {
    std::vector<std::string> line(headers.size());
    values.push_back(line);
  }
  std::stringstream ss;
  ss << std::fixed << std::setprecision (precision[current_column]) << value;
  values[current_line][current_column] = ss.str();
  current_column++;
  if (current_column == headers.size()) {
    current_column = 0;
    current_line++;
  }
  return *this;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
syd::PrintTable & syd::PrintTable::operator<<(const std::string & value)
{
  if (values.size() == current_line) {
    std::vector<std::string> line(headers.size());
    values.push_back(line);
  }
  // Trunc string if too big
  std::string v = value;
  if (v.size() > width[current_column]) {
    v = value.substr(0,width[current_column]-4)+"...";
  }

  values[current_line][current_column] = v;
  current_column++;
  if (current_column == headers.size()) {
    current_column = 0;
    current_line++;
  }
  return *this;
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::SkipLine()
{
  for(auto i=0; i<headers.size(); i++) *this << " ";
}
//------------------------------------------------------------------


//------------------------------------------------------------------
void syd::PrintTable::Print(std::ostream & out)
{
  for(auto i=0; i<headers.size(); i++) out << std::setw(width[i]) << headers[i];
  out << std::endl;
  for(auto i=0; i<values.size(); i++) {
    for(auto j=0; j<values[i].size(); j++) {
      out << std::setw(width[j]) << std::fixed << std::setprecision (precision[j]) << values[i][j];
    }
    out << std::endl;
  }
  out << std::flush;
}
//------------------------------------------------------------------
