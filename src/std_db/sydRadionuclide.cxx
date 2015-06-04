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
#include "sydDatabase.h"
#include "sydTable.h"
#include "sydInjection.h"
#include "sydInjection-odb.hxx"

// --------------------------------------------------------------------
syd::Radionuclide::Radionuclide():syd::TableElementBase()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Radionuclide::Set(const std::string & vname, double hl)
{
  name = vname;
  half_life_in_hours = hl;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Radionuclide::Set(std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    LOG(FATAL) << "Radionuclide Set require <name> <half_life_in_hours>";
  }
  Set(arg[0], atof(arg[1].c_str()));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Radionuclide::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << name << " "
     << half_life_in_hours;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::Radionuclide::operator==(const Radionuclide & p)
{
  return (id == p.id and
          name == p.name and
          half_life_in_hours == p.half_life_in_hours);
}
// --------------------------------------------------------------------


// --------------------------------------------------
void syd::Radionuclide::OnDelete(syd::Database * db)
{
  std::vector<syd::Injection> injections;
  db->Query<syd::Injection>(odb::query<syd::Injection>::radionuclide == id, injections);
  for(auto i:injections) db->AddToDeleteList(i);
}
// --------------------------------------------------