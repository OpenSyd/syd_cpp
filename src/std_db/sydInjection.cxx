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
syd::Injection::Injection():
  syd::Record(),
  syd::RecordWithComments()
{
  date = empty_value;
  activity_in_MBq = 0.0;
}
// --------------------------------------------------------------------


// --------------------------------------------------
std::string syd::Injection::ToString() const
{
  std::string name;
  if (patient == NULL) name = empty_value;
  else name = patient->name;
  std::string r;
  if (radionuclide == NULL) r = empty_value;
  else r = radionuclide->name;
  std::stringstream ss ;
  ss << id << " "
     << name << " "
     << r  << " "
     << date << " "
     << activity_in_MBq << " "
     << GetAllComments();
  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Injection::Set(const std::vector<std::string> & args)
{
  if (db_ == NULL) {
    LOG(FATAL) << "To use Set on an Injection, the db must be set before.";
  }
  if (args.size() < 4) {
    LOG(FATAL) << "Provide <patient> <radionuclide> <date> <activity_in_MBq>. "
               << std::endl
               << " <patient> can be the study_id or the name" << std::endl
               << " <radionuclide> can be a name or an id";
  }

  auto db = GetDatabase<syd::StandardDatabase>();
  std::string patient_name = args[0];
  auto p = db->FindPatient(patient_name);
  patient = p;
  syd::Radionuclide::pointer rad;
  odb::query<syd::Radionuclide> q = odb::query<syd::Radionuclide>::name == args[1] or
    odb::query<syd::Radionuclide>::id == atoi(args[1].c_str());
  try {
    rad = db->QueryOne<syd::Radionuclide>(q);
  } catch(std::exception & e) {
    LOG(FATAL) << "Error while creating the Injection, the radionuclide '"
               << args[1] << "' is not found (or several exist).";
  }
  radionuclide = rad;
  date = args[2];
  if (!IsDateValid(date)) {
    LOG(FATAL) << "The date is not valid for this injection: " << date;
  }
  activity_in_MBq = atof(args[3].c_str());
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Injection::DumpInTable(syd::PrintTable & ta) const
{
  std::string pname = empty_value;
  if (patient != NULL) pname = patient->name;
  std::string rad = empty_value;
  if (radionuclide != NULL) rad = radionuclide->name;
  ta.Set("id", id);
  ta.Set("p", pname);
  ta.Set("rad", rad);
  ta.Set("date", date);
  ta.Set("A(MBq)", activity_in_MBq, 2);
  ta.Set("com", GetAllComments());
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
double syd::Injection::GetLambdaDecayConstantInHours() const
{
  return radionuclide->GetLambdaDecayConstantInHours();
}
// --------------------------------------------------
