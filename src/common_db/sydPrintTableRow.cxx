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
#include "sydPrintTable.h"
#include "sydRecord.h"

//--------------------------------------------------------------------
syd::PrintTableRow::PrintTableRow(syd::PrintTable * t)
{
  table_ = t;
  values_.resize(10); // by default 10 col;
  std::fill(values_.begin(), values_.end(), "-");
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
syd::PrintTableRow::pointer
syd::PrintTableRow::New(syd::PrintTable * table)
{
  return std::make_shared<syd::PrintTableRow>(table);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::PrintTableRow::Set(int col, std::string value)
{
  if (col >= values_.size()) values_.resize(values_.size()+10);
  values_[col] = value;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::string syd::PrintTableRow::GetValue(int col) const
{
  if (col < values_.size()) return values_[col];
  else return "-";
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::PrintTableRow::Dump(const std::vector<int> & indices,
                              std::ostream & os) const
{
  for(auto col:indices) {
    auto column = table_->GetColumnInfo(col);
    if (!table_->GetSingleRowFlag())
      column->InstallStreamParameters(os);
    std::string s;
    // When the col does not exist, we used empty_value.
    if (col >= values_.size()) s = empty_value;
    else s = column->TruncateStringIfNeeded(values_[col]);
    os << s;
  }
  if (!table_->GetSingleRowFlag()) os << std::endl;
  else os << " ";
}
//--------------------------------------------------------------------
