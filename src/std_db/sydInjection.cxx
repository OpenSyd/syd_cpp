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
// #include "sydDatabase.h"
// #include "sydTable.h"
// #include "sydDicomSerie.h"
// #include "sydDicomSerie-odb.hxx"
// #include "sydTimepoint.h"
// #include "sydTimepoint-odb.hxx"

// --------------------------------------------------------------------
// syd::Injection::Injection():syd::TableElementBase()
// {
// }
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


void syd::Injection::Set(const syd::Database * d, const std::vector<std::string> & args)
{
  DD("Set Injection");
  if (args.size() < 4) {
    LOG(FATAL) << "Provide <patient> <radionuclide> <date> <activity_in_MBq>. "
               << std::endl
               << " <patient> can be the study_id or the name";
  }

  syd::StandardDatabase* db = (syd::StandardDatabase*)(d);
  std::string patient_name = args[0];
  auto p = db->FindPatient(patient_name);
  DD("find patient done");
  DD(p);
  patient = p;
  // radionuclide todo
  date = args[2];
  activity_in_MBq = atof(args[3].c_str());
}


// --------------------------------------------------
void syd::Injection::InitPrintTable(const syd::Database * db,
                                    syd::PrintTable & ta,
                                    const std::string & format) const
{
  DD("here initprinttable injection");
  ta.AddColumn("#id");
  ta.AddColumn("p", 15);
  ta.AddColumn("rad", 8);
  ta.AddColumn("date", 22);
  ta.AddColumn("Q(MBq)", 7);
}


void syd::Injection::DumpInTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  std::string pname = "unset_patient";
  if (patient != NULL) pname = patient->name;
  // std::string rad = "unset_radio";
  // if (radionuclide != NULL) rad = radionuclide->name;
  ta << id << pname << "Indium" << date << activity_in_MBq;
}


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
