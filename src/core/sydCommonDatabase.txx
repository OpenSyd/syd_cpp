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

//--------------------------------------------------------------------
template<class ElementType>
bool CompareTable(syd::Database * db1, syd::Database * db2)
{
  std::vector<ElementType> elements1;
  std::vector<ElementType> elements2;
  db1->Query(elements1);
  db2->Query(elements2);

  if (elements1.size() != elements2.size()) {
    return false;
  }

  for(auto i=0; i<elements1.size(); i++) {
    if (elements1[i] != elements2[i]) {
      return false;
    }
  }
  return true;
}
//--------------------------------------------------------------------
