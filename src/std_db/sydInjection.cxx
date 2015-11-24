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
#include "sydInjection.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
syd::Injection::Injection():syd::Record()
{
  patient = NULL;
  radionuclide = NULL;
  date = "unset";
  activity_in_MBq = 0.0;
}
// --------------------------------------------------------------------


// --------------------------------------------------
std::string syd::Injection::ToString() const
{
  std::string name;
  if (patient == NULL) name = "patient_not_set";
  else name = patient->name;
  std::string r;
  if (radionuclide == NULL) r = "radionuclide_not_set";
  else r = radionuclide->name;
  std::stringstream ss ;
  ss << id << " "
     << name << " "
     << r  << " "
     << date << " "
     << activity_in_MBq;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Injection::Set(const syd::Database * d, const std::vector<std::string> & args)
{
  if (args.size() < 4) {
    LOG(FATAL) << "Provide <patient> <radionuclide> <date> <activity_in_MBq>. "
               << std::endl
               << " <patient> can be the study_id or the name" << std::endl
               << " <radionuclide> can be a name or an id";
  }

  syd::StandardDatabase* db = (syd::StandardDatabase*)(d);
  std::string patient_name = args[0];
  auto p = db->FindPatient(patient_name);
  patient = p;
  syd::Radionuclide::pointer rad;
  odb::query<syd::Radionuclide> q = odb::query<syd::Radionuclide>::name == args[1] or
    odb::query<syd::Radionuclide>::id == atoi(args[1].c_str());
  try {
    db->QueryOne(rad, q);
  } catch(std::exception & e) {
    LOG(FATAL) << "Error while creating the Injection, the radionuclide '"
               << args[1] << "' is not found (or several exist).";
  }
  radionuclide = rad;
  date = args[2];
  if (!IsDateValid(date)) {
    LOG(FATAL) << "The date is not valid for this injection:" << this;
  }
  activity_in_MBq = atof(args[3].c_str());
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Injection::InitPrintTable(const syd::Database * db,
                                    syd::PrintTable & ta,
                                    const std::string & format) const
{
  if (format == "help") {
    std::cout << "Available formats for table 'Injection': " << std::endl
              << "\tdefault: id radionuclide date activity" << std::endl;
    //              << "\tinjection: add a column with the number of associated injections" << std::endl;
    return;
  }
  ta.AddColumn("#id");
  ta.AddColumn("p", 15);
  ta.AddColumn("rad", 15);
  ta.AddColumn("injec_date", 20);
  ta.AddColumn("A(MBq)", 10,2);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Injection::DumpInTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  std::string pname = "unset";
  if (patient != NULL) pname = patient->name;
  std::string rad = "unset";
  if (radionuclide != NULL) rad = radionuclide->name;
  ta << id << pname << rad << date << activity_in_MBq;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Injection::InitTable(syd::PrintTable & ta) const
{
  ta.AddColumn("id");
  ta.AddColumn("p");
  ta.AddColumn("rad");
  ta.AddColumn("date");
  auto & col = ta.AddColumn("A(MBq)");
  col.precision = 2;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Injection::DumpInTable(syd::PrintTable & ta) const
{
  std::string pname = "unset";
  if (patient != NULL) pname = patient->name;
  std::string rad = "unset";
  if (radionuclide != NULL) rad = radionuclide->name;
  ta.Set("id", id);
  ta.Set("p", pname);
  ta.Set("rad", rad);
  ta.Set("date", date);
  ta.Set("A(MBq)", activity_in_MBq);
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::Injection::IsEqual(const pointer p) const
{
  return (syd::Record::IsEqual(p) and
          patient->id == p->patient->id and
          radionuclide->id == p->radionuclide->id and
          date == p->date and
          activity_in_MBq == p->activity_in_MBq);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Injection::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Record::Callback(event, db);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Injection::Callback(odb::callback_event event, odb::database & db)
{
  syd::Record::Callback(event, db);
}
// --------------------------------------------------


// --------------------------------------------------
double syd::Injection::GetLambdaInHours() const
{
  return log(2.0)/radionuclide->half_life_in_hours;
}
// --------------------------------------------------
