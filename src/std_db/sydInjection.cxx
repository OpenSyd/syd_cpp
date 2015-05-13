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

// --------------------------------------------------------------------
syd::Injection::Injection():syd::TableElement()
{
  DD("const injection");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Injection::Injection(Patient & p,
                          const std::string & pradionuclide,
                          const std::string & pdate,
                          double activity):syd::Injection()
{
  Set(p, pradionuclide, pdate, activity);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Injection::~Injection()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Injection::Set(Patient & p,
                         const std::string & pradionuclide,
                         const std::string & pdate,
                         double activity)
{
  patient = std::make_shared<syd::Patient>(p);
  radionuclide = pradionuclide;
  if (!IsDateValid(pdate)) {
    LOG(FATAL) << "Error while using Injection::Set, the date is not valid: " << pdate;
  }
  date = date;
  activity_in_MBq = activity;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Injection::Injection(const syd::Injection & other)
{
  copy(other);
}
// --------------------------------------------------


// --------------------------------------------------
syd::Injection & syd::Injection::operator= (const syd::Injection & other) {
  if (this != &other) { copy(other); }
  return *this;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Injection::copy(const syd::Injection & t) {
  id = t.id;
  radionuclide = t.radionuclide;
  date = t.date;
  activity_in_MBq = t.activity_in_MBq;
  patient = t.patient;
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::Injection::ToString() const
{
  std::string name;
  if (patient == NULL) name = "patient_not_set";
  else name = patient->name;
  std::stringstream ss ;
  ss << id << " "
     << name << " "
     << radionuclide  << " "
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
          radionuclide == p.radionuclide and
          date == p.date and
          activity_in_MBq == p.activity_in_MBq);
}
// --------------------------------------------------
