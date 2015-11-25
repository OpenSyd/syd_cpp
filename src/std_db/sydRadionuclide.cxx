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
void syd::Radionuclide::InitTable(syd::PrintTable & ta) const
{
  ta.AddColumn("id");
  ta.AddColumn("name");
  ta.AddColumn("HL(h)");
  ta.AddColumn("element");
  ta.AddColumn("Z");
  ta.AddColumn("A");
  ta.AddColumn("metastable");
  auto & col = ta.AddColumn("Q-(keV)");
  col.precision = 2;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Radionuclide::DumpInTable(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("name", name);
  ta.Set("HL(h)", half_life_in_hours);
  ta.Set("element", element);
  ta.Set("Z", atomic_number);
  ta.Set("A", mass_number);
  ta.Set("metastable", (metastable? "Y":"N"));
  ta.Set("Q-(keV)", max_beta_minus_energy_in_kev);
}
// --------------------------------------------------
