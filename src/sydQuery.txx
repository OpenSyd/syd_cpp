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
void sydQuery::LoadVector(std::vector<T> & list, const odb::query<T> & q)
{
  odb::transaction t (db->begin());
  typedef odb::query<T> query;
  typedef odb::result<T> result;
  result r (db->query<T>(q));
  for(auto i=r.begin(); i != r.end(); i++) {
    T s;
    i.load(s);
    list.push_back(s);
  }
  t.commit();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class T>
void sydQuery::Load(T & t, const odb::query<T> & q)
{
  std::vector<T> list;
  LoadVector<T>(list, q);
  if (list.size() != 1)
    FATAL("I found " << list.size() << " results while expecting a single one."
          << "The SQL was : " << std::endl
          << mCurrentSQLQuery << std::endl);
  t = list[0];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class T>
bool sydQuery::LoadFirstIfExist(T & t, const odb::query<T> & q)
{
  std::vector<T> list;
  LoadVector<T>(list, q);
  if (list.size() == 0) return false;
  t = list[0];
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class T>
T & sydQuery::GetById(unsigned long id)
{
  static std::vector<T> tabletypes;
  static std::map<unsigned long, T> map;
  if (tabletypes.size() == 0) {
    LoadVector<T>(tabletypes, odb::query<T>::Id != -1); // all
    for(auto i=tabletypes.begin(); i<tabletypes.end(); i++) map[i->Id] = *i;
  }
  return map[id];
}
// --------------------------------------------------------------------
