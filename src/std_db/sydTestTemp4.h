
namespace syd {

  class FieldBase {
  public:
    typedef std::shared_ptr<FieldBase> pointer;
    typedef std::vector<pointer> vector;
    typedef std::function<std::string(pointer)> get_string_function_type;

    std::string name;
    std::string type_name;
    get_string_function_type GetValue;
    //virtual const get_string_function_type & GetValue = 0;
  };

  template<class RecordType, class FieldType>
    class Field: public FieldBase {
    typedef std::shared_ptr<Field<RecordType, FieldType> pointer;
    typedef std::vector<pointer> vector;
    //    typedef std::function<std::string(pointer)> get_string_specific_function_type;

    virtual const get_string_function_type & GetValue;
  };

  template<class RecordType, class FieldType>
    const get_string_function_type & Field<RecordType, Field>::GetValue {
    DDF();
  }



}
