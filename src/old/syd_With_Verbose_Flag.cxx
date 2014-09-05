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
#include "syd_With_Verbose_Flag.h"

 // --------------------------------------------------------------------
syd::With_Verbose_Flag::With_Verbose_Flag()
{
  m_VerboseFlag = false;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::With_Verbose_Flag::Set_VerboseFlag(bool b)
{
  m_VerboseFlag = b;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::With_Verbose_Flag::Get_VerboseFlag() const
{
  return m_VerboseFlag;
}
// --------------------------------------------------------------------
