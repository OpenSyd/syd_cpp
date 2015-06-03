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
#include "sydDatabase.h"
#include "sydTable.h"
#include "sydDicomSerie.h"
#include "sydDicomSerie-odb.hxx"
#include "sydTimepoint.h"
#include "sydTimepoint-odb.hxx"

// --------------------------------------------------------------------
syd::Injection::Injection():syd::TableElementBase()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Injection::Set(Patient & p,
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
bool syd::Injection::operator==(const Injection & p)
{
  return (id == p.id and
          *patient == *p.patient and
          *radionuclide == *p.radionuclide and
          date == p.date and
          activity_in_MBq == p.activity_in_MBq);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Injection::OnDelete(syd::Database * db)
{
  std::vector<syd::DicomSerie> dicomseries;
  db->Query<syd::DicomSerie>(odb::query<syd::DicomSerie>::injection == id, dicomseries);
  for(auto i:dicomseries) db->AddToDeleteList(i);

  std::vector<syd::Timepoint> tp;
  db->Query<syd::Timepoint>(odb::query<syd::Timepoint>::injection == id, tp);
  for(auto i:tp) db->AddToDeleteList(i);
}
// --------------------------------------------------
