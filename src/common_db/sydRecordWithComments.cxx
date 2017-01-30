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
#include "sydRecordWithComments.h"
#include "sydDatabase.h"

// --------------------------------------------------------------------
syd::RecordWithComments::RecordWithComments()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::RecordWithComments::GetAllComments() const
{
  if (comments.size() == 0) return empty_value;
  std::stringstream ss;
  int i = 0;
  for(auto c:comments) {
    if (comments.size() == 1) ss << c << " ";
    else ss << "(" << i << ") " << c << " ";
    ++i;
  }
  auto s = ss.str();
  return rtrim(s); // remove space at the end (or at start)
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RecordWithComments::
BuildMapOfFieldsFunctions(FieldFunctionMap & map)
{
  map["comments"] = [](pointer a) -> std::string { return a->GetAllComments(); };
}
// --------------------------------------------------------------------
