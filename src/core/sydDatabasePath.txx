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


// template<class TableElement>
// std::string GetAbsoluteFolder(const syd::Database * db, const TableElement & e)
// {
//   return db->GetDatabaseAbsoluteFolder()+PATH_SEPARATOR+syd::GetRelativeFolder(db, e);
// }

// template<class TableElement>
// std::string GetRelativeFolder(const syd::Database * db, const TableElement & e)
// {
//   LOG(FATAL) << "The table '" << TableElement::GetTableName() << "' does not have a relative folder. "
//              << "Need to specialize Database::GetRelativeFolder<" << TableElement::GetTableName() << ">";
// }

template<class TableElement>
std::string GetAbsoluteFilePath(const syd::Database * db, const TableElement & e)
{
  return db->GetAbsolutePath(syd::GetRelativeFilePath(db, e));
}

template<class TableElement>
std::string GetRelativeFilePath(const syd::Database * db, const TableElement & e)
{
  // FIXME : or default is empty ?
  LOG(FATAL) << "The table '" << TableElement::GetTableName() << "' does not have associated file. "
             << "Please, specialize GetRelativeFilePath<" << TableElement::GetTableName() << ">.";
}


template<class TableElement>
std::string CreateRelativeFolder(const syd::Database * db, const TableElement & e)
{
  // FIXME : or default is empty ?
  LOG(FATAL) << "The table '" << TableElement::GetTableName() << "' does not know how to determine a relative folder. "
             << "Please, specialize CreateRelativeFolder<" << TableElement::GetTableName() << ">.";
}


// --------------------------------------------------------------------
