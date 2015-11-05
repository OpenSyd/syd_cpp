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
  metastable = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------
std::string syd::Radionuclide::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << name << " "
     << half_life_in_hours << " "
     << element << " "
     << atomic_number << " "
     << mass_number << " "
     << (metastable? "metastable":"")
     << max_beta_minus_energy_in_kev;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::Radionuclide::IsEqual(const pointer p) const
{
  return (syd::Record::IsEqual(p) and name == p->name and
          half_life_in_hours == p->half_life_in_hours and
          element == p->element and
          atomic_number == p->atomic_number and
          mass_number == p->mass_number and
          metastable == p->metastable and
          max_beta_minus_energy_in_kev == p->max_beta_minus_energy_in_kev);
}
// --------------------------------------------------


// --------------------------------------------------
// void syd::Radionuclide::CopyFrom(const pointer p)
// {
//   syd::Record::CopyFrom(p);
//   name = p->name;
//   half_life_in_hours = p->half_life_in_hours;
//   element = p->element;
//   atomic_number = p->atomic_number;
//   mass_number = p->mass_number;
//   metastable = p->metastable;
//   max_beta_minus_energy_in_kev = p->max_beta_minus_energy_in_kev;
// }
// --------------------------------------------------


// --------------------------------------------------
void syd::Radionuclide::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  LOG(FATAL) << "To insert Radionuclide, please sydRadionuclideUpdate";
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
  ta.AddColumn("HL(h)", 10,2);
  ta.AddColumn("element", 10);
  ta.AddColumn("Z", 4);
  ta.AddColumn("A", 4);
  ta.AddColumn("metastable", 12);
  ta.AddColumn("Q-", 10,1);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Radionuclide::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  ta << id << name << half_life_in_hours << element << atomic_number << mass_number
     << (metastable? "Y":"N") << max_beta_minus_energy_in_kev;
}
// --------------------------------------------------
