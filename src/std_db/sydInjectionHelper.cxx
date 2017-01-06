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
#include "sydInjectionHelper.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
syd::Injection::pointer syd::CopyInjection(syd::Injection::pointer injection)
{
  auto db = injection->GetDatabase();
  auto output = db->New<syd::Injection>();
  output->patient = injection->patient;
  output->radionuclide = injection->radionuclide;
  output->date = injection->date;
  output->activity_in_MBq = injection->activity_in_MBq;
  return output;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Injection::pointer syd::FindInjection(const syd::Patient::pointer patient,
                                           const std::string & rad_name_or_inj_id)
{
  auto db = patient->GetDatabase();
  syd::Injection::pointer injection;
  odb::query<syd::Injection> q =
    odb::query<syd::Injection>::patient == patient->id
    and (odb::query<syd::Injection>::radionuclide->name == rad_name_or_inj_id.c_str() or
         odb::query<syd::Injection>::id == atoi(rad_name_or_inj_id.c_str()));

  try {
    db->QueryOne(injection, q);
  } catch(std::exception & e) {
    EXCEPTION("Error in FindInjection for patient " << patient->name
              << ", with param: " << rad_name_or_inj_id << std::endl
              << "Error message is: " << e.what());
  }
  return injection;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Injection::vector syd::GetSimilarInjection(syd::StandardDatabase * db,
                                                 const syd::Injection::pointer injection)
{
  syd::Injection::vector injections;
  odb::query<syd::Injection> q =
    odb::query<syd::Injection>::patient == injection->patient->id and
    odb::query<syd::Injection>::radionuclide == injection->radionuclide->id and
    odb::query<syd::Injection>::date == injection->date and
    odb::query<syd::Injection>::activity_in_MBq == injection->activity_in_MBq and
    odb::query<syd::Injection>::id != injection->id;
  try {
    db->Query(injections, q);
  } catch(std::exception & e) {
    EXCEPTION("Error in GetSimilarInjection for injection " << injection
              << "Error message is: " << e.what());
  }
  return injections;
}
// --------------------------------------------------------------------
