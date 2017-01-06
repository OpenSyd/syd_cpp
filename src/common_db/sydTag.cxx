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
#include "sydTag.h"
#include "sydPrintTable.h"

// std
#include <set>

DEFINE_TABLE_IMPL(Tag);

// --------------------------------------------------------------------
syd::Tag::Tag():syd::Record()
{
  label = empty_value;
  description = empty_value;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Tag::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << label << " "
     << description;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Tag::Set(const std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    LOG(FATAL) << "To insert a Tag, please set <label> <description>";
  }
  label = arg[0];
  description = arg[1];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Tag::DumpInTable(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("label", label);
  ta.Set("description", description);
}
// --------------------------------------------------------------------
