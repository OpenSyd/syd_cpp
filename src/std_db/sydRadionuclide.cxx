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
#include "sydRadionuclide.h"

// --------------------------------------------------------------------
syd::Radionuclide::Radionuclide():syd::Record()
{
  name = "unset";
  half_life_in_hours = 0.0;
}
// --------------------------------------------------------------------


// --------------------------------------------------
std::string syd::Radionuclide::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << name << " "
     << half_life_in_hours;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::Radionuclide::IsEqual(const pointer p) const
{
  return (syd::Record::IsEqual(p) and name == p->name and
          half_life_in_hours == p->half_life_in_hours);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Radionuclide::CopyFrom(const pointer p)
{
  syd::Record::CopyFrom(p);
  name = p->name;
  half_life_in_hours = p->half_life_in_hours;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Radionuclide::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    sydLOG(FATAL) << "To insert patient, please set <name> <half_life_in_hours>";
  }
  name = arg[0];
  half_life_in_hours = atof(arg[1].c_str());
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Radionuclide::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  if (format == "help") {
    std::cout << "Available formats for table 'Radionuclide': " << std::endl
              << "\tdefault: id name half_life_in_hours" << std::endl;
    return;
  }
  ta.AddColumn("#id");
  ta.AddColumn("name", 15);
  ta.AddColumn("half_life(h)", 10,2);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Radionuclide::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  ta << id << name << half_life_in_hours;
}
// --------------------------------------------------
