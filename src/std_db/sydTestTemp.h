


// everything that is static
template<class RecordType>
class MyBaseTrait  { // <--- NOT possible to make it inherit from MyBase due to odb)

 public:
  static void GetStaticFieldFunction(); // Needed for default static
  static void GetStaticDefaultFieldFunction();
  static int value;
  static RecordType * singleton;
  static RecordType * Singleton() { if (!singleton) return new RecordType; else return singleton;} 
};

class MyBase  { // Base data
 public:
  virtual void GetFieldFunction() { DDF(); }
  //  static void GetStaticFieldFunction() {DD("no");};// = 0;
  //  static void GetStaticFieldFunction();// {DD("no"); }
  int id; // odb
};


/* class MyRecord: public MyBaseTrait<MyRecord> { */
  
/* }; */

class MyPatient:
  public virtual MyBaseTrait<MyPatient>,
  public virtual MyBase {
 public:
  virtual void GetFieldFunction() { GetStaticFieldFunction(); }
  static void GetStaticFieldFunction();
   // real implementation specific --> no need to declare thanks to specialized implementation/
};


// Can be automatized 
// MUST be before implementation of MyPatient::GetFieldFunction
template<>
int MyBaseTrait<MyBase>::value = 000;
template<>
int MyBaseTrait<MyPatient>::value = 666;
template<>
MyPatient * MyBaseTrait<MyPatient>::singleton = nullptr;

// Default (id raw)
template<class RecordType>
void MyBaseTrait<RecordType>::GetStaticFieldFunction() {
  DD("DEFAULT : MyBaseTrait<RecordType>::GetStaticFieldFunction");
  GetStaticDefaultFieldFunction();
}

template<class RecordType>
void MyBaseTrait<RecordType>::GetStaticDefaultFieldFunction() {
  DD("MyBaseTrait<RecordType>::GetStaticDefaultFieldFunction");
  value = 666;
  DD(value);
}

// Specialization no need to declare
template<>
void MyBaseTrait<MyPatient>::GetStaticFieldFunction() {
  DD("MyBaseTrait<MyBaseTrait>::GetStaticFieldFunction");
  GetStaticDefaultFieldFunction(); // <-- how to call default ? 
  ++value;
  DD(value);
}

void MyPatient::GetStaticFieldFunction()
{
  MyBaseTrait<MyPatient>::GetStaticFieldFunction();
}

// Can be automatized BUT Needed. 
/* void MyPatient::GetFieldFunction() { */
/*   DD("MyPatient::GetFieldFunction"); */
/*   MyPatient::GetStaticFieldFunction(); */
/* } */

/*void MyPatient::GetStaticFieldFunction() {
  DD("MyPatient::GetStaticFieldFunction");
  MyBaseTrait<MyPatient>::GetStaticFieldFunction();
  ++value;
  DD(value);
}
*/

