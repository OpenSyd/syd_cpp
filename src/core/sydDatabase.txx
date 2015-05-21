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
template<class ElementType>
Table<ElementType> * syd::Database::GetTable()
{
  return (Table<ElementType>*)GetTable(ElementType::GetTableName());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ElementType>
void syd::Database::AddTable()
{
  // No exception handling here, fatal error if fail.
  if (db_ == NULL) {
    LOG(FATAL) << "Could not AddTable, open a db before";
  }
  std::string tablename = ElementType::GetTableName();
  auto it = map.find(tablename);
  if (it != map.end()) {
    LOG(FATAL) << "When creating the database, a table with the same name '" << tablename
               << "' already exist.";
  }
  // Also check with uppercase and lowercase
  std::string str = tablename;
  std::transform(str.begin(), str.end(),str.begin(), ::toupper);
  it = map.find(str);
  if (it != map.end()) {
    LOG(FATAL) << "When creating the database, a table with the same name '" << tablename
               << "' already exist.";
  }
  std::transform(str.begin(), str.end(),str.begin(), ::tolower);
  it = map.find(str);
  if (it != map.end()) {
    LOG(FATAL) << "When creating the database, a table with the same name '" << tablename
               << "' already exist.";
  }
  map[tablename] = new Table<ElementType>(this, db_);
  map_lowercase[str] = map[tablename];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ElementType>
void syd::Database::Query(const odb::query<ElementType> & q, std::vector<ElementType> & list) {
  GetTable<ElementType>()->Query(q,list);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ElementType>
void syd::Database::Query(std::vector<ElementType> & list)
{
  GetTable<ElementType>()->Query(list);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ElementType>
ElementType syd::Database::QueryOne(const odb::query<ElementType> & q)
{
  return GetTable<ElementType>()->QueryOne(q);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ElementType>
ElementType syd::Database::QueryOne(IdType id)
{
  return GetTable<ElementType>()->QueryOne(id);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ElementType>
unsigned int syd::Database::Count(const odb::query<ElementType> & q)
{
  return GetTable<ElementType>()->Count(q);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ElementType>
bool syd::Database::IfExist(IdType id)
{
  return GetTable<ElementType>()->IfExist(id);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ElementType>
void syd::Database::Insert(ElementType & r)
{
  GetTable<ElementType>()->Insert(r);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ElementType>
void syd::Database::Insert(std::vector<ElementType*> & r)
{
  GetTable<ElementType>()->Insert(r);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class ElementType>
void syd::Database::Update(ElementType & r)
{
  GetTable<ElementType>()->Update(r);
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
template<class ElementType>
void syd::Database::Update(std::vector<ElementType*> & r)
{
  GetTable<ElementType>()->Update(r);
}
// --------------------------------------------------------------------

/*
// --------------------------------------------------------------------
template<class TableType>
void syd::Database::Erase(T & r)
{
  odb::transaction t (db_->begin());
  db_->erase<T>(r);
  t.commit();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableType>
void syd::Database::Erase(std::vector<T> & r)
{
  odb::transaction t (db_->begin());
  for(auto i: r) db_->erase<T>(*i);
  t.commit();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableType>
T syd::Database::GetById(IdType id)
{
  odb::transaction transaction (db_->begin());
  typedef odb::query<TableType> query;
  typedef odb::result<T> result;
  result r (db_->query<TableType>(odb::query<TableType>::id == id));
  if (r.begin() != r.end()) {
    T s;
    r.begin().load(s);
    transaction.commit();
    return s;
  }
  transaction.commit();
  LOG(FATAL) << "Error element with id = " << id << " does not exist in "
             << get_name() << " (" << GetType() << ")";
  T s;
  return s; // fake return
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableType>
bool syd::Database::GetIfExist(odb::query<TableType> q, T & t)
{
  odb::transaction transaction (db_->begin());
  typedef odb::query<TableType> query;
  typedef odb::result<T> result;
  result r (db_->query<TableType>(q));
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
template<class TableType>
bool syd::Database::GetOrInsert(odb::query<TableType> q, T & t)
{
  bool b = GetIfExist<T>(q, t);
  if (!b) { //Create
    Insert(t);
    return true;
  }
  return false;
}
// --------------------------------------------------------------------
*/
