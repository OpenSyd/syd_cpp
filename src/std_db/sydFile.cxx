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
#include "sydFile.h"

// --------------------------------------------------
syd::File::File():TableElement()
{
  // default value
  filename = "";
  path = "";
  md5 = "";
}
// --------------------------------------------------


// --------------------------------------------------
syd::File::File(const File & other):TableElement(other)
{
  copy(other);
}
// --------------------------------------------------


// --------------------------------------------------
syd::File & syd::File::operator= (const syd::File & other)
{
  if (this != &other) { copy(other); }
  return *this;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::File::copy(const syd::File & t)
{
  id = t.id;
  filename = t.filename;
  path = t.path;
  md5 = t.md5;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::File::SetValues(std::vector<std::string> & arg)
{
  LOG(FATAL) << "No SetValues for table 'File'.";
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::File::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << path << " "
     << filename << " "
     << (md5 == "" ? "no_md5":"md5");
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::File::operator==(const File & p)
{
  return (id == p.id and
          path == p.filename and
          md5 == p.md5);
}
// --------------------------------------------------
