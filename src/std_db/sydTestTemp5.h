
namespace syd {

  /*
  template<class RecordType>
    void RecordTraits<RecordType>::GetField(std::string name) {

    auto f = map[name]; // specific field function

    auto gf = [f](syd::Record::pointer gr) -> std::string {
      auto r = std::static_pointer_cast<RecordType>(gr);
      return f(r);
    };
    }*/

  typedef std::function<std::string(syd::Record::pointer)> FieldFunc;


  class Table {
  public:

    // ------------------------------
    void Build(syd::Record::vector records, std::string columns) {
      DDF();
      values_.resize(records.size());
      if (records.size() == 0) return;
      auto table_name = records[0]->GetTableName();
      DD(table_name);
      auto db = records[0]->GetDatabase();
      auto field = db->GetField(table_name, columns);
      //      auto fields = db->GetFields(table_name, columns);
      Build(records, field);
    }
    // ------------------------------


    // ------------------------------
    // FIXME template Build<T>(T::vector ...)
    void Build(syd::Record::vector records, FieldFunc f) {
      DDF();
      values_.resize(records.size());
      int i=0; // row
      for(auto & r:records) {
        values_[i].resize(1);
        values_[i][0] = f(r);
        /*
          int j=0; // column
          values_[i].resize(fields.size());
          for(auto & f:fields) {
          values_[i][j] = f(r); // get the value
          ++j
          }
        */
        ++i;
      }
    }
    // ------------------------------



    // ------------------------------
    void Print(std::ostream & os) {
      DDF();
      //for(auto & h:headers_) h->Print(os);
      //for(auto & r:rows_) r->Print(os);
      for(auto & row:values_) {
        for(auto & col:row) os << col << " ";
        os << std::endl;
      }
    }
    // ------------------------------


    std::vector<std::vector<std::string>> values_;

  };




}
