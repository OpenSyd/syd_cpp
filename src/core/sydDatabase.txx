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
template<class Record>
void syd::Database::New(std::shared_ptr<Record> & record) const
{
  DD("Database::New");
  // std::shared_ptr<Record> r(new Record);
  // record = r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class Record>
void syd::Database::Insert(std::shared_ptr<Record> record)
{
  DD("Database Insert");
  DD(record);
  try {
    odb::transaction t (db_->begin());
    db_->persist(*record);
    //    db_->update(r);
    t.commit();
  }
  catch (const odb::exception& e) {
    LOG(FATAL) << "Cannot insert the element: <"
               << record << "> in the table '" << "FIXME"
               << "'. The error is: "  << e.what()
               << std::endl << "And last sql query is: "
               << std::endl << GetLastSQLQuery();
  }
  DD("Insert is done");
}
// --------------------------------------------------------------------


/*
// --------------------------------------------------------------------
template<class TableElement>
Table<TableElement> * syd::Database::GetTable() const
{
  return (Table<TableElement>*)GetTable(TableElement::GetTableName());
}
// --------------------------------------------------------------------
*/


// --------------------------------------------------------------------
/*
template<class TableElement>
void syd::Database::AddTable()
{
  // No exception handling here, fatal error if fail.
  if (db_ == NULL) {
    LOG(FATAL) << "Could not AddTable, open a db before";
  }
  std::string tablename = TableElement::GetTableName();
  // auto it = map.find(tablename);
  // if (it != map.end()) {
  //   LOG(FATAL) << "When creating the database, a table with the same name '" << tablename
  //              << "' already exist.";
  // }
  // Also check with uppercase and lowercase
  std::string str = tablename;
  // std::transform(str.begin(), str.end(),str.begin(), ::toupper);
  // it = map.find(str);
  // if (it != map.end()) {
  //   LOG(FATAL) << "When creating the database, a table with the same name '" << tablename
  //              << "' already exist.";
  // }
  std::transform(str.begin(), str.end(),str.begin(), ::tolower);
  auto it = map_lowercase.find(str);
  if (it != map_lowercase.end()) {
    LOG(FATAL) << "When creating the database, a table with the same name '" << tablename
               << "' already exist.";
  }


  auto * t = new Table<TableElement>;
  t->SetSQLDatabase(db_);
  t->SetDatabase(this);
  t->Initialization();
  map[tablename] = t;//new Table<TableElement>(this, db_);
  map_lowercase[str] = t;//map[tablename];
}
*/
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/*template<class Table>
void syd::Database::AddTableTT()
{
  // No exception handling here, fatal error if fail.
  if (db_ == NULL) {
    LOG(FATAL) << "Could not AddTable, open a db before";
  }
  std::string tablename = Table::GetTableName();
  DD(tablename);
  std::string str = tablename;
  std::transform(str.begin(), str.end(),str.begin(), ::tolower);
  auto it = map_lowercase.find(str);
  if (it != map_lowercase.end()) {
    LOG(FATAL) << "When creating the database, a table with the same name '" << tablename
               << "' already exist.";
  }
  Table * t = new Table;
  t->SetSQLDatabase(db_);
  t->SetDatabase(this);
  t->Initialization();
  map[tablename] = t;
  map_lowercase[str] = t;
}
*/
// --------------------------------------------------------------------
