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

#ifndef SYDINJECTIONHELPER_H
#define SYDINJECTIONHELPER_H

// syd
#include "sydInjection.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// This file contains helpers function that are hopefully helpful
  /// for syd::Injection table

  // Make a copy of an injection, it is *NOT* inserted in the db
  syd::Injection::pointer CopyInjection(syd::Injection::pointer input);

  /// Look for an injection for a patient, given an id or a
  /// radionuclide name and a patient
  syd::Injection::pointer FindInjection(const syd::Patient::pointer patient,
                                        const std::string & rad_name_or_inj_id);

  /// Check if a similar injection already exist (same date, quantity,
  /// rad, but different id)
  syd::Injection::vector GetSimilarInjection(syd::StandardDatabase * db,
                                             const syd::Injection::pointer injection);

}
// --------------------------------------------------------------------

#endif
