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

template<class Record>
void syd::Database::AddTable()
{
  // No exception handling here, fatal error if fail.
  if (db_ == NULL) {
    LOG(FATAL) << "Could not AddTable, open a db before";
  }
  std::string tablename = Record::GetStaticTableName();
  std::string str = tablename;
  std::transform(str.begin(), str.end(),str.begin(), ::tolower);
  auto it = map_lowercase.find(str);
  if (it != map_lowercase.end()) {
    LOG(FATAL) << "When creating the database, a table with the same name '" << tablename
               << "' already exist.";
  }


  auto * t = new Table<Record>;
  t->db_ = this; // FIXME
  map[tablename] = t;
  map_lowercase[str] = t;
}
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


// --------------------------------------------------------------------
template<class Record>
void syd::Database::QueryOne(std::shared_ptr<Record> & record, const odb::query<Record> & q) const
{
  DD("QueryOne");
  try {
    odb::transaction transaction (db_->begin());
    DD("here");
    auto r = db_->query_one<Record>(q);
    if (r.get() == 0) {
      DD("bug");
      EXCEPTION("Error in sql query for the table '" << Record::GetStaticTableName() << "'"
                << std::endl << "And last sql query is: "
                << std::endl << GetLastSQLQuery());
    }
    record = r;
    // DD(record);
    transaction.commit();
  }
  catch (const odb::exception& e) {
    LOG(FATAL) << "Error in sql query for the table '" << record->GetTableName()
               << "', message is: " << e.what()
               << std::endl << "And last sql query is: "
               << std::endl << GetLastSQLQuery();
  }
}
