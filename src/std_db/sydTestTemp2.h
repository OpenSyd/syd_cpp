
class MyBaseTraitBase {
 public:
  virtual void GetSingletonFieldFunction() = 0;
};


// everything that is static
template<class RecordType>
class MyBaseTrait: public MyBaseTraitBase { // <--- NOT possible to make it inherit from MyBase due to odb)
 public:
  virtual void GetSingletonFieldFunction();
  static void GetStaticFieldFunction(); // Needed for default static
  static void GetStaticDefaultFieldFunction(); // to be call if reuse
  static int value;
  static MyBaseTraitBase * singleton;
};

class MyBase {
 public:
  virtual void GetFieldFunction();
  MyBaseTraitBase * GetSingleton() { return MyBaseTrait<MyBase>::singleton; }
  int id; // odb
};

class MyPatient: public MyBase {
 public:
  virtual void GetFieldFunction();
  MyBaseTraitBase * GetSingleton() { return MyBaseTrait<MyPatient>::singleton; }
};


// Default initialisation
template<class RecordType>
int MyBaseTrait<RecordType>::value = 666;

template<class RecordType>
MyBaseTraitBase * MyBaseTrait<RecordType>::singleton = new MyBaseTrait<RecordType>;

// Default implementation
template<class RecordType>
void MyBaseTrait<RecordType>::GetStaticFieldFunction() {
  DD("DEFAULT : MyBaseTrait<RecordType>::GetStaticFieldFunction");
  GetStaticDefaultFieldFunction();
}

// Common defaut
template<class RecordType>
void MyBaseTrait<RecordType>::GetStaticDefaultFieldFunction() {
  DD("MyBaseTrait<RecordType>::GetStaticDefaultFieldFunction");
  value = 666;
  DD(value);
}

template<class RecordType>
void MyBaseTrait<RecordType>::GetSingletonFieldFunction() {
  DD("MyBaseTrait<RecordType>::GetSingletonFieldFunction");
  value = 666;
  DD(value);
}




// Specialization no need to declare
// XXXX ====> MUST be implemented or default, no need declared
template<>
void MyBaseTrait<MyPatient>::GetStaticFieldFunction() {
  DD("MyBaseTrait<MyBaseTrait>::GetStaticFieldFunction");
  GetStaticDefaultFieldFunction(); // <-- how to call default ? 
  ++value;
  DD(value);
}

// XXXX ==> needed AFTER previous specialization
void MyBase::GetFieldFunction() { MyBaseTrait<MyBase>::GetStaticFieldFunction(); }
void MyPatient::GetFieldFunction() { MyBaseTrait<MyPatient>::GetStaticFieldFunction(); }


template<>
void MyBaseTrait<MyPatient>::GetSingletonFieldFunction() {
  DD("MyBaseTrait<MyPatient>::GetSingletonFieldFunction");
  ++value;
  DD(value);
}

