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
  syd::Injection::pointer output;
  auto db = injection->GetDatabase<syd::StandardDatabase>();
  db->New(output);
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
  auto db = patient->GetDatabase<syd::StandardDatabase>();
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
