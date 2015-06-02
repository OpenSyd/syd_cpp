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

// --------------------------------------------------------------------
syd::Tag::Tag():syd::TableElementBase()
{
  label = "unamed_tag";
  description = "no_description";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Tag::Set(std::vector<std::string> & arg)
{
  if (arg.size() == 0) {
    LOG(FATAL) << "Could not insert a Tag without argument, at least set the 'label'.";
  }
  label = arg[0];
  if (arg.size() > 1) description = arg[1];
}
// --------------------------------------------------------------------


// --------------------------------------------------
std::string syd::Tag::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << label << " "
     << description;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::Tag::operator==(const Tag & p)
{
  return (id == p.id and
          label == p.label and
          description == p.description);
}
// --------------------------------------------------
