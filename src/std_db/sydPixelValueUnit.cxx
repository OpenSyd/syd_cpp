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
#include "sydPixelValueUnit.h"

// std
#include <set>

// --------------------------------------------------
syd::PixelValueUnit::PixelValueUnit():syd::Record()
{
  name = "unset";
  description = "unset";
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::PixelValueUnit::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << name << " "
     << description;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::PixelValueUnit::IsEqual(const pointer p) const
{
  return (syd::Record::IsEqual(p) and
          name == p->name and
          description == p->description);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::PixelValueUnit::CopyFrom(const pointer p)
{
  syd::Record::CopyFrom(p);
  name = p->name;
  description = p->description;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::PixelValueUnit::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    LOG(FATAL) << "To insert a PixelValueUnit, please set <name> <description>";
  }
  name = arg[0];
  description = arg[1];
}
// --------------------------------------------------



// --------------------------------------------------
void syd::PixelValueUnit::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  if (format == "help") {
    std::cout << "Available formats for table 'PixelValueUnit': " << std::endl
              << "\tdefault: id name description" << std::endl;
    return;
  }
  ta.AddColumn("#id");
  ta.AddColumn("name", 20);
  ta.AddColumn("description", 50);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::PixelValueUnit::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  ta << id << name << description;
}
// --------------------------------------------------
