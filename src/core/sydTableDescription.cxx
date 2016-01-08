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
#include "sydTableDescription.h"

// --------------------------------------------------------------------
syd::FieldDescription &
syd::TableDescription::GetField(std::string field_name)
{
  DDF();
  DD(field_name);

  auto it = std::find_if(fields_.begin(), fields_.end(),
                         [&field_name](syd::FieldDescription & t)
                         { return t.GetName() == field_name; } );
  if (it == fields_.end()) {
    LOG(FATAL) << "field not found";
  }
  return *it;
}
// --------------------------------------------------------------------
