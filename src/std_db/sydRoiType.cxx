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
syd::RoiType::RoiType():TableElementBase()
{
  name = "unnamed_roi";
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
bool syd::RoiType::operator==(const RoiType & p)
{
  bool b = (id == p.id and
            name == p.name and
            description == p.description);
  return b;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiType::Set(std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    LOG(FATAL) << "To insert roitype, please set <name> <description>";
  }
  name = arg[0];
  description = arg[1];
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiType::OnDelete(syd::Database * db)
{
  LOG(FATAL) << "TODO OnDelete for RoiType not yet implemented.";
// Look for all RoiMaskImage that use this roitype ...
  //  db->AddToDeleteList(*image);
}
// --------------------------------------------------
