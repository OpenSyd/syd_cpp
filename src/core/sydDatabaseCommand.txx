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

// --------------------------------------------------------------------
template<class T>
T * syd::DatabaseCommand::OpenNewDatabase(std::string name)
{
  // Create new database
  T * t = new T(name);

  // Get the filename and foldername and open the db
  t->OpenDatabase(get_db_filename(name), get_db_folder(name));

  // Add to the list of db
  databases_.push_back(t);

  // return created db
  return t;
}
// --------------------------------------------------------------------
