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
#include "sydPrintTableColumnInfo.h"

//--------------------------------------------------------------------
syd::PrintTableColumnInfo::PrintTableColumnInfo(int i)
{
  index_ = i;
  width_ = 1;
  max_width_ = 50;
  precision_ = 0;
  use_user_precision_ = false;
  color_ = resetColor;
  indexColor = greenColor;
  headerColor = magentaColor;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
int syd::PrintTableColumnInfo::SetPrecision(int i)
{
  precision_ = i;
  use_user_precision_ = true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::PrintTableColumnInfo::SetName(std::string n)
{
  name_ = n;
  if (name_ =="id") color_ = indexColor;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
syd::PrintTableColumnInfo::pointer
syd::PrintTableColumnInfo::New(int i)
{
  return std::make_shared<syd::PrintTableColumnInfo>(i);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::PrintTableColumnInfo::InstallStreamParameters(std::ostringstream & os) const
{
  os << color_ << std::setw(width_);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::string syd::PrintTableColumnInfo::TruncateStringIfNeeded(const std::string & s) const
{
  // limit the width (minus 1 for adding a space between column)
  if (s.size() >= width_) return s.substr(0, width_-1);
  else return s;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::PrintTableColumnInfo::UpdateWidth(const std::string & v)
{
  // Consider a width 1 char larger than the value size
  // With a max at max_width_
  if (v.size() >= width_)
    width_ = std::min((int)v.size()+1, max_width_);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::string syd::PrintTableColumnInfo::GetStringValue(double value, int precision)
{
  std::ostringstream os;
  if (precision != -1 and !use_user_precision_) precision_ = precision_;
  os << std::fixed << std::setprecision(precision_) << value;
  return os.str();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::PrintTableColumnInfo::DumpHeader(std::ostringstream & os)
{
  std::ostringstream temp;
  temp << index_+1 << " " << name_ << "  "; // two spaces because start with #
  auto s = temp.str();
  UpdateWidth(s);
  os << std::setw(width_) << s;
}
//--------------------------------------------------------------------
