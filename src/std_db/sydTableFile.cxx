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
#include "sydTableFile.h"
#include "sydDatabase.h"

// --------------------------------------------------------------------

// template<>
// std::string GetRelativeFolder<syd::File>(syd::Database * db, const syd::File & e)
// {
//   return file.path;
// }

template<>
std::string syd::GetRelativeFilePath(const syd::Database * db, const syd::File & file)
{
  return file.path+PATH_SEPARATOR+file.filename;
}
