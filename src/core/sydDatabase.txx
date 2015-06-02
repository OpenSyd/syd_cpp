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
template<class TableElement>
Table<TableElement> * syd::Database::GetTable()
{
  return (Table<TableElement>*)GetTable(TableElement::GetTableName());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Database::Delete(IdType id)
{
  GetTable<TableElement>()->Delete(id);
  // TableElement elem = QueryOne<TableElement>(id);
  // return Delete(TableElement::GetTableName(), elem);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Database::Delete(TableElement & e)
{
  GetTable<TableElement>()->Delete(e);
  //  return Delete(TableElement::GetTableName(), e);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
int syd::Database::Delete(std::vector<TableElement> & ve)
{
  // for(auto e:ve) n += AddToDeleteList(TableElement::GetTableName(), e);
  GetTable<TableElement>()->Delete(ve);
  DeleteCurrentList();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
int syd::Database::Delete(std::vector<IdType> & ids)
{
  GetTable<TableElement>()->Delete(ids);
  DeleteCurrentList();
  // for(auto id:ids) n += AddToDeleteList(TableElement::GetTableName(), id);
  // return DeleteCurrentList();
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Database::AddTable()
{
  // No exception handling here, fatal error if fail.
  if (db_ == NULL) {
    LOG(FATAL) << "Could not AddTable, open a db before";
  }
  std::string tablename = TableElement::GetTableName();
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
  map[tablename] = new Table<TableElement>(this, db_);
  map_lowercase[str] = map[tablename];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Database::Query(const odb::query<TableElement> & q, std::vector<TableElement> & list) {
  GetTable<TableElement>()->Query(q,list);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Database::Query(std::vector<TableElement> & list)
{
  GetTable<TableElement>()->Query(list);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
TableElement syd::Database::QueryOne(const odb::query<TableElement> & q)
{
  return GetTable<TableElement>()->QueryOne(q);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
TableElement syd::Database::QueryOne(IdType id)
{
  //  return dynamic_cast<TableElement&>(GetTable<TableElement>()->QueryOne(id));
  return GetTable<TableElement>()->QueryOne(id);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
unsigned int syd::Database::Count(const odb::query<TableElement> & q)
{
  return GetTable<TableElement>()->Count(q);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
bool syd::Database::IfExist(IdType id)
{
  return GetTable<TableElement>()->IfExist(id);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Database::Insert(TableElement & r)
{
  GetTable<TableElement>()->Insert(r);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Database::Insert(std::vector<TableElement*> & r)
{
  GetTable<TableElement>()->Insert(r);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Database::Update(TableElement & r)
{
  GetTable<TableElement>()->Update(r);
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
template<class TableElement>
void syd::Database::Update(std::vector<TableElement*> & r)
{
  GetTable<TableElement>()->Update(r);
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


//--------------------------------------------------------------------
template<class TableElement>
bool syd::Database::TableIsEqual(syd::Database * db)
{
  std::vector<TableElement> elements1;
  std::vector<TableElement> elements2;
  Query(elements1);
  db->Query(elements2);

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
