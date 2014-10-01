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
template<class DatabaseType>
std::shared_ptr<DatabaseType> syd::Database::OpenDatabaseType(std::string name)
{
  std::shared_ptr<Database> d = syd::Database::OpenDatabase(name);
  if (d->get_typename() != DatabaseType::typename_) {
    LOG(FATAL) << "Error while attempting to read the database '" << name
               << "' which is of type '" << d->get_typename()
               << "' while expecting type '" << DatabaseType::typename_ << "'";
  }
  return std::dynamic_pointer_cast<DatabaseType>(d);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// Warning : do not clear the list, append
template<class T>
void syd::Database::LoadVector(std::vector<T> & list, const odb::query<T> & q)
{
  odb::transaction transaction (db_->begin());
  typedef odb::query<T> query;
  typedef odb::result<T> result;
  result r (db_->query<T>(q));
  for(auto i = r.begin(); i != r.end(); i++) {
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
  odb::transaction t (db_->begin());
  db_->persist(r);
  db_->update(r);
  t.commit();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class T>
void syd::Database::Update(T & r)
{
  odb::transaction t (db_->begin());
  db_->update(r);
  t.commit();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class T>
void syd::Database::Erase(T & r)
{
  odb::transaction t (db_->begin());
  db_->erase<T>(r);
  t.commit();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class T>
void syd::Database::Erase(std::vector<T> & r)
{
  odb::transaction t (db_->begin());
  for(auto i: r) db_->erase<T>(*i);
  t.commit();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class T>
T syd::Database::GetById(IdType id)
{
  odb::transaction transaction (db_->begin());
  typedef odb::query<T> query;
  typedef odb::result<T> result;
  result r (db_->query<T>(odb::query<T>::id == id));
  if (r.begin() != r.end()) {
    T s;
    r.begin().load(s);
    transaction.commit();
    return s;
  }
  transaction.commit();
  LOG(FATAL) << "Error element with id = " << id << " does not exist in "
             << get_name() << " (" << get_typename() << ")";
  T s;
  return s; // fake return
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class T>
bool syd::Database::GetIfExist(odb::query<T> q, T & t)
{
  odb::transaction transaction (db_->begin());
  typedef odb::query<T> query;
  typedef odb::result<T> result;
  result r (db_->query<T>(q));
  bool b = false;
  if (r.begin() != r.end()) {
    b = true;
    r.begin().load(t); // load the first element
  }
  transaction.commit();
  return b;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class T>
bool syd::Database::GetOrInsert(odb::query<T> q, T & t)
{
  bool b = GetIfExist<T>(q, t);
  if (!b) { //Create
    Insert(t);
    return true;
  }
  return false;
}
// --------------------------------------------------------------------
