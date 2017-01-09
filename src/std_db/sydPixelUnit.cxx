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
#include "sydPixelUnit.h"
#include "sydPrintTable.h"
#include "sydStandardDatabase.h"
#include "sydRecordTraits.h"

// std
#include <set>

DEFINE_TABLE_IMPL(PixelUnit);

// --------------------------------------------------
syd::PixelUnit::PixelUnit():syd::Record()
{
  name = empty_value;
  description = empty_value;
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::PixelUnit::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << name << " "
     << description;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
void syd::PixelUnit::Set(const std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    LOG(FATAL) << "To insert a PixelUnit, please set <name> <description>";
  }
  name = arg[0];
  description = arg[1];
}
// --------------------------------------------------


// --------------------------------------------------
void syd::PixelUnit::DumpInTable(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("name", name);
  ta.Set("description", description);
}
// --------------------------------------------------
