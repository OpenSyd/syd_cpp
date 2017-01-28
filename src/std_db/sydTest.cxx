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

// syd
#include "sydTest_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"
#include "sydTableOfRecords.h"
#include "sydPrintTable.h"
#include "sydField.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydTest, 1);

  // Load plugin and db
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // -----------------------------------------------------------------

  {
    auto a = odb::object_traits_impl<syd::Patient, odb::id_common>::table_name;
    DD(a);
  }

  syd::Image::pointer image;
  db->QueryOne(image, 11);
  DD(image);
  auto table_name = image->GetTableName();
  syd::Record::pointer record = image;
  DD(record);

  DD("----------------------------");

  //  auto traits = syd::RecordTraits<syd::Image>::GetTraits();
  auto field = db->NewField("Image","acquisition_date");
  DD(field->get(record));

  // field = db->NewField("Image", "patient");
  // std::cout << field->name << " " << field->get(record) << std::endl;

  // field = db->NewField("Image", "modality");
  // std::cout << field->name << " " << field->get(record) << std::endl;

  field = db->NewField("Image", "bidon3");
  std::cout << field->name << " " << field->get(record) << std::endl;
  DD(field);
  DD(field->ToString());

  // field = db->NewField("Image", "injection.patient");
  // std::cout << field->name << " " << field->get(record) << std::endl;

  // field = db->NewField("Image", "injection.patient.name");
  // std::cout << field->name << " " << field->get(record) << std::endl;

  field = db->NewField("Image", "patient.name");
  std::cout << field->name << " " << field->get(record) << std::endl;

  DD(field);
  DD(field->ToString());

  auto t = syd::RecordTraits<syd::Image>::GetTraits();
  auto map = t->GetFieldsMap();
  syd::RecordTraitsBase::FieldMapType mm;
  mm = map; // copy ?
  mm["toto"] = field;
  field = mm["toto"];
  std::cout << field->name << " " << field->get(record) << std::endl;


  // field = db->NewField("Image", "bidon");
  // std::cout << field->name << " " << field->get(record) << std::endl;
  // DD(field);
  // DD(field->ToString());

  /*
  auto field = db->NewField("Image", "id");
  DD(field->get(record));
  DD(field->name);
  DD(field->type);

  field = db->NewField("Injection", "date");
  DD(field->get(image->injection));
  DD(field->name);
  DD(field->type);

  field = db->NewField("Image", "injection.date");
  DD(field->get(record));
  DD(field->name);
  DD(field->type);
  */

  /*
  syd::Init();

  auto field = GetField(db, "Image", "id");
  DD(field->get(record));
  DD(field->name);
  DD(field->type);
  //DD(field.ro); // ?? read only ?
  // FIXME  field.set(record, "12");

  field = GetField(db, "Image", "date");
  DD(field->get(record));
 
    syd::Patient::pointer patient = image->patient;
    DD(patient);
    DD(field->get(patient));

  field = GetField(db, "Image", "injection");
  field->precision = 5; //SetPrecision(5);
  //field.SetAbbrev("ac"); // needed ?
  DD(field->get(record));

  field = GetField(db, "Image", "injection.activity_in_MBq");
  field->precision = 5; //SetPrecision(5);
  //field.SetAbbrev("ac"); // needed ?
  // field.IsValid ?
  DD(field->get(record));

  exit(0);
  */

  /*
  // Create fields
  syd::Field<syd::Image> image_date;
  image_date.ff = [](syd::Image::pointer a) -> std::string & { return a->acquisition_date; };
  syd::Field<syd::Image> image_dim;
  image_dim.ff = [](syd::Image::pointer a) -> double & { return a->dimension; };
  syd::Field<syd::Image> image_patient;
  image_patient.ff = [](syd::Image::pointer a) -> syd::Record::pointer { return a->patient; };
  {
  auto f = image_dim.ff;
  image_dim.f =  [f](syd::Record::pointer a) -> double & {
  auto r = std::static_pointer_cast<syd::Image>(a);
  auto s = boost::get<syd::Field<syd::Image>::FuncDouble>(&f);
  return (*s)(r); };

  auto g = image_date.ff;
  image_date.f =  [g](syd::Record::pointer a) -> std::string & {
  auto r = std::static_pointer_cast<syd::Image>(a);
  auto s = boost::get<syd::Field<syd::Image>::FuncString>(&g);
  return (*s)(r); };

  auto h = image_patient.ff;
  image_patient.f =  [h](syd::Record::pointer a) -> syd::Record::pointer {
  auto r = std::static_pointer_cast<syd::Image>(a);
  auto s = boost::get<syd::Field<syd::Image>::FuncRecord>(&h);
  return (*s)(r); };
  }

  // Create the map
  std::map<std::string, syd::FieldBase> map;
  map["date"] = image_date;
  map["dim"] = image_dim;
  map["patient"] = image_patient;

  DD("get the function");
  {
  auto f = map["date"].f;
  auto s = *boost::get<syd::FieldBase::FuncString>(&f);
  DD("apply date");
  auto value = s(record);
  DD(value);
  value = "I am here";
  DD(record);
  }
  {
  auto f = map["dim"].f;
  auto s = boost::get<syd::FieldBase::FuncDouble>(&f);
  DD("apply dim");
  DD((*s)(record));     // get
  }
  {
  auto f = map["date"].f;
  auto s = boost::get<syd::FieldBase::FuncDouble>(&f);
  DD("apply false date");
  if (s) {
  DD((*s)(record));     // get
  }
  }
  {
  auto f = map["dim"].f;
  auto s = boost::get<syd::FieldBase::FuncString>(&f);
  DD("apply false dim");
  if (s) {
  DD((*s)(record));     // get
  }
  }


  {
  auto f = map["patient"].f;
  auto s = boost::get<syd::FieldBase::FuncRecord>(&f);
  DD("apply patient");
  if (s) {
  auto r = (*s)(record);     // get
  DD(r);
  }
  }


  //f(record) = "toto" // set


  exit(0);
  */
  // -----------------------------------------------------------------
  /*if (0) {
    syd::Image::vector images;
    db->Query(images);
    DD(images.size());
    auto table_name = images[0]->GetTableName();
    auto columns = "raw";

    syd::Record::vector records;
    for(auto &im:images) records.push_back(im);

    // Step1
    //auto columns_info = TableBuildColumns(table_name, columns);

    // Step2
    syd::PrintTable table;
    auto fields = db->GetFields(table_name, "raw");
    table.Build(records, fields);
    table.Print(std::cout);
    //  table.Build(records, "patient.id");
    // table.SetHeaderFlag(true); // options
    // Step3
    //table.Print(std::cout);
    }*/

  // -----------------------------------------------------------------
  /*  if (0) {

    syd::Image::vector images;
    db->Query(images);
    DD(images.size());
    DD(images[0]->GetTableName());
    for(auto i:images) std::cout << i->id << " "; std::cout << std::endl;
    db->Sort<syd::Image>(images);
    db->Sort(images);
    for(auto i:images) std::cout << i->id << " "; std::cout << std::endl;

    syd::Record::vector records;
    db->Query(records, "Image");
    DD(records.size());
    for(auto i:records) std::cout << i->id << " "; std::cout << std::endl;
    db->Sort(records, "Image", "date");
    for(auto i:records) std::cout << i->id << " "; std::cout << std::endl;

  }
  */
  // -----------------------------------------------------------------


  DD("end");
  // This is the end, my friend.
}
// --------------------------------------------------------------------
