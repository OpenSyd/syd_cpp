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
syd::Injection::Injection():syd::Record("")
{
  patient = NULL;
  // radionuclide = NULL; FIXME
  date = "unset_date";
  activity_in_MBq = 0.0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/*void syd::Injection::Set(Patient & p,
                         Radionuclide & pr,
                         const std::string & pdate,
                         double activity)
{
  patient = std::make_shared<syd::Patient>(p);
  radionuclide = std::make_shared<syd::Radionuclide>(pr);
  if (!IsDateValid(pdate)) {
    LOG(FATAL) << "Error while using Injection::Set, the date is not valid: " << pdate;
  }
  date = date;
  activity_in_MBq = activity;
}
*/
// --------------------------------------------------------------------


// --------------------------------------------------
std::string syd::Injection::ToString() const
{
  std::string name;
  if (patient == NULL) name = "patient_not_set";
  else name = patient->name;
  std::string r="fixme";
  //if (radionuclide == NULL) r = "radionuclide_not_set";
  //else r = radionuclide->name;
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
  // radionuclide todo
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
  ta.AddColumn("rad", 8);
  ta.AddColumn("injec_date", 20);
  ta.AddColumn("A(MBq)", 7,2);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Injection::DumpInTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  std::string pname = "unset_patient";
  if (patient != NULL) pname = patient->name;
  // std::string rad = "unset_radio";
  // if (radionuclide != NULL) rad = radionuclide->name;
  ta << id << pname << "Indium" << date << activity_in_MBq;
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::Injection::IsEqual(const pointer p) const
{
  return (syd::Record::IsEqual(p) and
          patient->IsEqual(p->patient) and
          //          patient.IsEqual(p.patient) and radionuclide FIXME
          date == p->date and
          activity_in_MBq == p->activity_in_MBq);
}
// --------------------------------------------------




// --------------------------------------------------
// bool syd::Injection::operator==(const Injection & p)
// {
//   return (id == p.id and
//           *patient == *p.patient and
//           *radionuclide == *p.radionuclide and
//           date == p.date and
//           activity_in_MBq == p.activity_in_MBq);
// }
// --------------------------------------------------


// --------------------------------------------------
// void syd::Injection::OnDelete(syd::Database * db)
// {
//   DD(" injection OnDelete");
//   /*
//   std::vector<syd::DicomSerie> dicomseries;
//   db->Query<syd::DicomSerie>(odb::query<syd::DicomSerie>::injection == id, dicomseries);
//   for(auto i:dicomseries) db->AddToDeleteList(i);

//   std::vector<syd::Timepoint> tp;
//   db->Query<syd::Timepoint>(odb::query<syd::Timepoint>::injection == id, tp);
//   for(auto i:tp) db->AddToDeleteList(i);
//   */
// }
// --------------------------------------------------
