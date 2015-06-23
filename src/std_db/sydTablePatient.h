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

#ifndef SYDTABLEPATIENT_H
#define SYDTABLEPATIENT_H

// syd
#include "sydPatient.h"
#include "sydDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// Return the patient by name or study_id. Exception if not found. Require table with fields 'name' and 'study_id'. Required a template.
  template<class Patient>
  void FindPatientByNameOrStudyId(syd::Patient & p, syd::Database * db, std::string & arg);

  #include "sydTablePatient.txx"
}
// --------------------------------------------------------------------

#endif
