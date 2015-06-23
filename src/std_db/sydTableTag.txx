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
template<class Tag>
void FindTags(std::vector<Tag> & tags, syd::Database * db, const std::string & names)
{
  std::vector<std::string> words;
  syd::GetWords(names, words);
  odb::query<Tag> q = odb::query<Tag>::label.in_range(words.begin(), words.end());
  db->Query<Tag>(q, tags);
}
// --------------------------------------------------------------------
