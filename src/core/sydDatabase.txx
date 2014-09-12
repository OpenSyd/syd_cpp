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
void syd::Database::LoadVector(std::vector<T> & list, const odb::query<T> & q)
{
  odb::transaction transaction (db->begin());
  typedef odb::query<T> query;
  typedef odb::result<T> result;
  result r (db->query<T>(q));
  for(auto i=r.begin(); i != r.end(); i++) {
    T s;
    i.load(s);
    list.push_back(s);
  }
  transaction.commit();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class T>
void syd::Database::Insert(T & r)
{
  odb::transaction t (db->begin());
  db->persist(r);
  db->update(r);
  t.commit();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class T>
void syd::Database::Update(T & r)
{
  odb::transaction t (db->begin());
  db->update(r);
  t.commit();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class T>
T & syd::Database::GetById(IdType id)
{
  static std::vector<T> tabletypes;
  static std::map<IdType, T> map;
  // The first time we build the map
  if (tabletypes.size() == 0) {
    LoadVector<T>(tabletypes, odb::query<T>::id != -1); // all
    for(auto i=tabletypes.begin(); i<tabletypes.end(); i++) map[i->id] = *i;
  }
  return map[id];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class T>
bool syd::Database::GetIfExist(odb::query<T> q, T & t)
{
  // Check if already exist
  std::vector<T> elements;
  LoadVector<T>(elements, q);
  if (elements.size() == 0) return false;
  t = elements[0];
  return true;
}
// --------------------------------------------------------------------
