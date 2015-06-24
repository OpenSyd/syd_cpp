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
syd::Table<TableElement>::Table(syd::Database * db, odb::sqlite::database * d)
:TableBase(d), database_(db)
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::Insert(TableElement & r)
{
  try {
    odb::transaction t (db_->begin());
    db_->persist(r);
    db_->update(r);
    t.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Cannot insert the element: "
              << r.ToString() << std::endl
              << " in the table '" << syd::Table<TableElement>::GetTableName()
              << "'. Maybe a element with same unique field already exist or a foreign constraint is not fulfill ?" << std::endl
              << "The odb exception is: "  << e.what()
              << std::endl << "And last sql query is: " << database_->GetLastSQLQuery());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::Insert(std::vector<TableElement*> & r)
{
  try {
    odb::transaction t (db_->begin());
    for(auto x:r) {
      db_->persist(*x);
      db_->update(*x);
    }
    t.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Cannot insert " << r.size() << " elements in the table '"
              << TableElement::GetTableName()
              << "'. Maybe a element with same unique field already exist or a foreign constraint is not fulfill ?" << std::endl
              << "The odb exception is: "  << e.what()
              << std::endl << "And last sql query is: "
              << std::endl << database_->GetLastSQLQuery());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::AddToDeleteList(std::vector<syd::IdType> & ids)
{
  if (ids.size() == 0) return;
  std::vector<TableElement> ve;
  odb::query<TableElement> q = odb::query<TableElement>::id == ids[0];
  for(auto i:ids) q = q or odb::query<TableElement>::id == i;
  Query(q, ve);
  for(auto e:ve) database_->AddToDeleteList(e);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::AddToDeleteList(syd::IdType id)
{
  TableElement e = QueryOne(odb::query<TableElement>::id == id);
  database_->AddToDeleteList(e);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::AddAllToDeleteList()
{
  std::vector<TableElement> ve;
  Query(ve);
  for(auto e:ve) database_->AddToDeleteList(e);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::Erase(TableElementBase * elem)
{
  db_->erase(*dynamic_cast<TableElement*>(elem));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
unsigned int syd::Table<TableElement>::GetNumberOfElements()
{
  std::vector<TableElement> list;
  Query(list);
  return list.size();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// Warning : do not clear the vector, append
template<class TableElement>
void syd::Table<TableElement>::Query(const odb::query<TableElement> & q,
                                     std::vector<TableElement> & list)
{
  try {
    odb::transaction transaction (db_->begin());
    typedef odb::query<TableElement> query;
    typedef odb::result<TableElement> result;
    result r (db_->query<TableElement>(q));
    for(auto i = r.begin(); i != r.end(); i++) {
      TableElement s;
      i.load(s);
      list.push_back(s);
    }
    transaction.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Error in sql query for the table '" << TableElement::GetTableName()
              << "', odb exception is: " << e.what()
              << std::endl << "And last sql query is: "
              << std::endl << database_->GetLastSQLQuery());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::Query(const std::vector<IdType> & ids,
                                     std::vector<TableElement> & list)
{
  if (ids.size()==0) return;
  typedef odb::query<TableElement> query;
  query q(odb::query<TableElement>::id.in_range(ids.begin(), ids.end()));
  Query(q,list);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::Query(std::vector<TableElement> & list)
{
  odb::query<TableElement> q;
  return Query(q, list);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
TableElement syd::Table<TableElement>::QueryOne(const odb::query<TableElement> & q)
{
  std::vector<TableElement> elements;
  Query(q,elements);
  if (elements.size() == 1) return elements[0];
  if (elements.size() == 0) {
    EXCEPTION("Error while QueryOne in table '" << TableElement::GetTableName()
              << "', no element that match the query. Query is: " << std::endl
              << database_->GetLastSQLQuery());
  }
  EXCEPTION("Error while QueryOne in table '" << TableElement::GetTableName()
            << "', I found " << elements.size()
            << " element(s) that match the query. Query is: "
            << std::endl << database_->GetLastSQLQuery());
  TableElement e;
  return e; // to avoid compilation warning
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
TableElement syd::Table<TableElement>::QueryOne(IdType id)
{
  odb::query<TableElement> q = odb::query<TableElement>::id == id;
  return QueryOne(q);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
bool syd::Table<TableElement>::IfExist(IdType id)
{
  odb::query<TableElement> q = odb::query<TableElement>::id == id;
  return (Count(q) == 1); // Cannot be > 1
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
unsigned int syd::Table<TableElement>::Count(const odb::query<TableElement> & q)
{
  int n=0;
  try {
    odb::transaction transaction (db_->begin());
    typedef odb::query<TableElement> query;
    typedef odb::result<TableElement> result;
    result r (db_->query<TableElement>(q));
    //  DD(r.size()); // not supported by sqlite
    for(auto i = r.begin(); i != r.end(); i++) ++n;
    transaction.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Error in sql query for the table '" << TableElement::GetTableName()
              << "', odb exception is: " << e.what()
              << std::endl << "And last sql query is: "
              << std::endl << database_->GetLastSQLQuery());
  }
  return n;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::Dump(std::ostream & os, const std::string & format)
{
  // Get all elements
  std::vector<TableElement> elements;
  Query(elements);
  Dump(os, format, elements);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::Dump(std::ostream & os, const std::string & format, const std::vector<syd::IdType> & ids)
{
  // Get all elements with the given ids. Ignore if not find
  std::vector<TableElement> elements;
  Query(ids, elements);
  Dump(os, format, elements);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::Dump(std::ostream & os, const std::string & format, const std::vector<TableElement> & elements)
{
  // Default dump
  if (format == "help") {
    os << "No 'format' defined for the table '" << TableElement::GetTableName() << "'";
    return;
  }
  if (format != "") {
    os << "Unknown format for table '" << TableElement::GetTableName() << "', use default format.";
    return;
  }
  for(auto e:elements) {
    os << e << std::endl;
  }
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::Find(std::vector<syd::IdType> & ids,
                                    const std::vector<std::string> & pattern,
                                    const std::vector<std::string> & exclude)
{
  std::vector<TableElement> elements;
  Query(elements);
  for(auto e:elements) {
    std::string s(e.ToLargeString());
    bool b = true;
    for(auto p:pattern) {
      if (s.find(p) == std::string::npos) { b = false; break; } // not match the pattern
    }
    for(auto p:exclude) {
      if (s.find(p) != std::string::npos) { b = false; break; } // exclude
    }
    if (b) ids.push_back(e.id);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::Update(TableElement & r)
{
  try {
    odb::transaction t (db_->begin());
    db_->update(r);
    t.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Cannot update the element: "
              << r.ToString() << std::endl
              << " in the table '" << TableElement::GetTableName()
              << "'. The error is: "  << e.what()
              << std::endl << "And last sql query is: "
              << std::endl << database_->GetLastSQLQuery());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::Update(std::vector<TableElement*> & r)
{
  try {
    odb::transaction t (db_->begin());
    for(auto x:r) db_->update(*x);
    t.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Cannot update " << r.size()
              << " elements in the table '" << TableElement::GetTableName()
              << "'. The error is: "  << e.what()
              << std::endl << "And last sql query is: "
              << std::endl << database_->GetLastSQLQuery());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TableElement>
void syd::Table<TableElement>::Update(std::vector<TableElement> & r)
{
  try {
    odb::transaction t (db_->begin());
    for(auto x:r) {
      db_->update(x);
    }
    t.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Cannot update " << r.size()
              << " elements in the table '" << TableElement::GetTableName()
              << "'. The error is: "  << e.what()
              << std::endl << "And last sql query is: "
              << std::endl << database_->GetLastSQLQuery());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/// default impltementation of insert, must be overloaded
template<class TableElement>
TableElement * syd::Table<TableElement>::InsertFromArg(std::vector<std::string> & arg)
{
  TableElement e;
  e.Set(arg);
  Insert(e);
  return new TableElement(e);
}
// --------------------------------------------------------------------
