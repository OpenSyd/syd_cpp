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
#include "sydRoiType.h"

// --------------------------------------------------------------------
syd::RoiType::RoiType():Record()
{
  name = "unset";
  description = "unset";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::RoiType::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << name << " '"
     << description << "'";
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
void syd::RoiType::CopyFrom(const pointer p)
{
  syd::Record::CopyFrom(p);
  name = p->name;
  description = p->description;
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::RoiType::IsEqual(const pointer p) const
{
  bool b = (syd::Record::IsEqual(p) and
            name == p->name and
            description == p->description);
  return b;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiType::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    sydLOG(FATAL) << "To insert roitype, please set <name> <description>";
  }
  name = arg[0];
  description = arg[1];
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiType::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  if (format == "help") {
    std::cout << "Available formats for table 'RoiType': " << std::endl
              << "\tdefault: id name study_id weight dicom" << std::endl;
    return;
  }
  ta.AddColumn("#id");
  ta.AddColumn("name", 20);
  ta.AddColumn("description", 80);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiType::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  ta << id << name << description;
}
// --------------------------------------------------
