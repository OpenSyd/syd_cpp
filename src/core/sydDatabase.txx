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
template<class RecordType>
void syd::Database::New(std::shared_ptr<RecordType> & record) const
{
  record = RecordType::New();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
std::shared_ptr<RecordType> syd::Database::New() const
{
  return RecordType::New();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Insert(std::shared_ptr<RecordType> record)
{
  try {
    odb::transaction t (db_->begin());
    db_->persist(*record);
    //    db_->update(r);
    t.commit();
  }
  catch (const odb::exception& e) {
    std::string n="";
    if (record != 0) n = record->GetTableName();
    LOG(FATAL) << "Cannot insert the element: <"
               << record << "> in the table '" << n
               << "'. The error is: "  << e.what()
               << std::endl << "And last sql query is: "
               << std::endl << GetLastSQLQuery();
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::AddTable()
{
  // No exception handling here, fatal error if fail.
  if (db_ == NULL) {
    LOG(FATAL) << "Could not AddTable, open a db before";
  }
  std::string tablename = RecordType::GetStaticTableName();
  std::string str = tablename;
  std::transform(str.begin(), str.end(),str.begin(), ::tolower);
  auto it = map_lowercase.find(str);
  if (it != map_lowercase.end()) {
    LOG(FATAL) << "When creating the database, a table with the same name '" << tablename
               << "' already exist.";
  }
  auto * t = new Table<RecordType>(this);
  //  t->db_ = this; // FIXME
  map[tablename] = t;
  map_lowercase[str] = t;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::QueryOne(std::shared_ptr<RecordType> & record,
                             const odb::query<RecordType> & q) const
{
  try {
    odb::transaction transaction (db_->begin());
    auto r = db_->query_one<RecordType>(q);
    if (r.get() == 0) {
      EXCEPTION("No matching record in QueryOne(q) for the table '" << RecordType::GetStaticTableName()
                << ". Last sql query is: "
                << std::endl << GetLastSQLQuery());
    }
    record = r;
    // DD(record);
    transaction.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Error in QueryOne(q) for the table '" << record->GetTableName()
              << "', cannot find the record. Last sql query is: "
              << std::endl << GetLastSQLQuery());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::QueryOne(std::shared_ptr<RecordType> & record, const IdType & id) const
{
  try {
    odb::transaction transaction (db_->begin());
    record = db_->load<RecordType>(id);
    transaction.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Error in QueryOne sql query for the table '" << RecordType::GetStaticTableName()
              << "' and id = " << id << std::endl
              << "\t odb message: " << e.what() << std::endl
              << "\t last sql query: " << GetLastSQLQuery());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Query(std::vector<std::shared_ptr<RecordType>> & records,
                          const odb::query<RecordType> & q) const
{
  try {
    odb::transaction transaction (db_->begin());
    typedef odb::result<RecordType> result;
    result r(db_->query<RecordType>(q));
    for(auto i = r.begin(); i != r.end(); i++) {
      std::shared_ptr<RecordType> s = New<RecordType>();
      i.load(*s);
      records.push_back(s);
    }
    transaction.commit();
  }
  catch (const odb::exception& e) {
    EXCEPTION("Error during Query(r, q) for the table '" << RecordType::GetStaticTableName()
              << "'. Last sql query is: "
              << std::endl << GetLastSQLQuery());
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Query(std::vector<std::shared_ptr<RecordType>> & records) const
{
  odb::query<RecordType> q;
  Query(records, q);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
void syd::Database::Query(std::vector<std::shared_ptr<RecordType>> & records,
                          const std::vector<syd::IdType> & ids) const
{
  if (ids.size() == 0) return;
  odb::query<RecordType> q(odb::query<RecordType>::id == ids[0]);
  for(auto id:ids) {
    q = q or odb::query<RecordType>::id == id;
  }
  Query(records, q);
}
// --------------------------------------------------------------------
