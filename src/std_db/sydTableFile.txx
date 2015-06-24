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

#include "sydDatabase.h"

// --------------------------------------------------------------------
template<class File>
void InsertNewFile(File & file, syd::Database * db, const std::string filename, const std::string & relative_folder)
{
  // Check if already exist
  std::string p = db->GetDatabaseAbsoluteFolder()+PATH_SEPARATOR+relative_folder+PATH_SEPARATOR+filename;
  DD(p);
  if (syd::FileExists(p)) {
    EXCEPTION("Error while creating File, filename already exist: " << p);
  }
  file.filename = filename;
  file.path = relative_folder;
  db->Insert(file);
}
// --------------------------------------------------------------------
