
namespace temp {

  class Record;

  class RecordTraitsBase {
  public:
    typedef std::function<std::string(std::shared_ptr<temp::Record>)> GetFieldFunctionType;
    typedef std::map<std::string, GetFieldFunctionType> GetFieldFunctionMapType;

    // Pure virtual, should be reimpl in avery RecordType
    virtual std::string GetTableName() const = 0;
    virtual void SetFieldFunctions(GetFieldFunctionMapType & map) = 0;
    virtual const GetFieldFunctionType & GetFieldFunction(std::string field_name) {
      std::cout << "GetFieldFunction : " << field_name << " " << GetTableName() << std::endl;
      auto map = GetFieldFunctionMap();
      auto it = map.find(field_name);
      if (it != map.end()) return it->second;
      DD("not found");
      EXCEPTION("Error");
    }

    // To do in RecordTraits
    virtual const GetFieldFunctionMapType & GetFieldFunctionMap() = 0;
    //virtual std::vector<std::shared_ptr<FieldBase>> & GetFields() = 0;

  };


  // everything static here, specific to a record type
  template<class RecordType>
    class RecordTraits: public RecordTraitsBase {
  public:
    typedef RecordTraitsBase::GetFieldFunctionType GetFieldFunctionType;
    typedef RecordTraitsBase::GetFieldFunctionMapType GetFieldFunctionMapType;

    static RecordTraitsBase * GetTraits() { return singleton; }
    virtual std::string GetTableName() const { return table_name_; }

    // Default impl of pure virtual
    virtual const GetFieldFunctionMapType & GetFieldFunctionMap() {
      static bool already_here = false;
      std::cout << "GetFieldFunctionMap " << table_name_ << std::endl;
      if (already_here) return field_function_map_;
      std::cout << "GetFieldFunctionMap --> BUILD" << std::endl;
      SetFieldFunctions(field_function_map_);
      already_here = true;
      return field_function_map_;
    }

    // To specialize
    virtual void SetFieldFunctions(GetFieldFunctionMapType & map) {
      DD(table_name_);
      DDF();
    }


    /* static std::vector<std::shared_ptr<FieldBase>> fields; */
    /* virtual std::vector<std::shared_ptr<FieldBase>> & GetFields() { */
    /*   return fields; */
    /* } */


    static RecordTraitsBase * singleton;
    static std::string table_name_;
    static GetFieldFunctionMapType field_function_map_;
  };

  class Record {
  public:
    virtual RecordTraitsBase * GetTraits() { return RecordTraits<Record>::singleton; } 
    typedef RecordTraitsBase::GetFieldFunctionType GetFieldFunctionType;

    // duplicate of RecordsTraits to ease access
    std::string GetTableName() { return RecordTraits<Record>::GetTraits()->GetTableName(); }
    const GetFieldFunctionType & GetFieldFunction(std::string field_name) { return RecordTraits<Record>::GetTraits()->GetFieldFunction(field_name); }

    /* virtual std::vector<std::shared_ptr<FieldBase>> & GetFields() { */
    /*   return GetTraits()->GetFields(); */
    /* } */


    int id; // odb
  };

  class Patient: public Record {
  public:
    // Needed:
    virtual RecordTraitsBase * GetTraits() { return RecordTraits<Patient>::singleton; } 

    // Not needed but convenient FIXME ---> maybe not to avoid too much function
    static std::string GetTableName() { return RecordTraits<Patient>::GetTraits()->GetTableName(); }

    std::string name;
  };

  // Default singleton initialisation
  template<class RecordType>
    RecordTraitsBase * RecordTraits<RecordType>::singleton = new RecordTraits<RecordType>;

  // Default implementation
  template<class RecordType>
    std::string RecordTraits<RecordType>::table_name_ = "default";

  // Declare map
  template<class RecordType>
    typename RecordTraits<RecordType>::GetFieldFunctionMapType
    RecordTraits<RecordType>::field_function_map_;

  //----------------------------------------------------------

  // Specialisation for Patient
  template<>
    std::string RecordTraits<Record>::table_name_ = "Record";

  template<>
    void RecordTraits<Record>::SetFieldFunctions(GetFieldFunctionMapType & map) {
    std::cout << "SetFieldFunctions of Record" << std::endl;
    map["id"] = [](std::shared_ptr<Record> r) { return std::to_string(r->id); };
  }

  template<>
    std::string RecordTraits<Patient>::table_name_ = "Patient";

  /*
    const RecordTraitsBase::GetFieldFunctionType &
    specific(const std::function<std::string(std::shared_ptr<temp::Patient>)> & f) {
    auto g = [f](std::shared_ptr<Record> r) {
    auto p = std::dynamic_pointer_cast<Patient>(r);
    if (p == nullptr) {
    LOG(WARNING) << "error function type";
    return std::string("");
    };
    return g;
    }
    }
  */

  template<>
    void RecordTraits<Patient>::SetFieldFunctions(GetFieldFunctionMapType & map) {
    std::cout << "SetFieldFunctions of Patient" << std::endl;
    temp::RecordTraits<Record>::GetTraits()->SetFieldFunctions(map);
    map["name"] = [](std::shared_ptr<Record> r) {
      auto p = std::dynamic_pointer_cast<Patient>(r);
      if (p == nullptr) {
        LOG(WARNING) << "error function type";
        return std::string("");
      }
      return p->name;
    };
  }

  //----------------------------------------------------------
  template<class RecordType>
    void PrintTable(std::vector<std::shared_ptr<RecordType>> records) {
    DDF();
    if (records.size() == 0) return;
    auto traits = RecordType::GetTraits();
    auto fields = traits->GetDefaultFieldFunctions();
    for(auto & r:records) {
      for(auto & f:fields)
        std::cout << f(r) << " " ;
      std::cout << std::endl;
    }
  }


}
