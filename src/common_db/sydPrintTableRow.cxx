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
#include "sydPrintTableRow.h"
#include "sydPrintTable2.h"

//--------------------------------------------------------------------
syd::PrintTableRow::PrintTableRow(syd::PrintTable2 * t)
{
  table = t;
  values.resize(10); // by default 10 col;
  std::fill(values.begin(), values.end(), "-");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
syd::PrintTableRow::pointer
syd::PrintTableRow::New(syd::PrintTable2 * table)
{
  return std::make_shared<syd::PrintTableRow>(table);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::PrintTableRow::Set(int col, std::string value)
{
  if (col >= values.size()) values.resize(values.size()+10);
  values[col] = value;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::string syd::PrintTableRow::GetValue(int col) const
{
  if (col < values.size()) return values[col];
  else return "-";
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::string syd::PrintTableRow::ToString() const
{
  std::stringstream os;
  for(auto v:values) os << v << " ";
  return os.str();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::PrintTableRow::Dump(const std::vector<int> & indices,
                              std::ostringstream & os) const
{

  for(auto col:indices) {
    auto column = table->GetColumnInfo(col);
    column->InstallStreamParameters(os);
    auto s = column->TruncateStringIfNeeded(values[col]);
    os << s;
  }
  os << std::endl;
}
//--------------------------------------------------------------------
