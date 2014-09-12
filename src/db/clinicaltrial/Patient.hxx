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

#include <string>
#include <iostream>
#include <odb/core.hxx>

typedef unsigned int IdType;

// --------------------------------------------------------------------
#pragma db object
class Patient
{
public:

#pragma db id auto
  IdType        id;
  std::string   name;
  IdType        synfrizz_id;
  double        weight_in_kg;
  std::string   dicom_patient_name;
  //  std::string   dicom_patient_id;
  std::string   path;
  bool          was_treated;
  std::string   injection_date;
  std::string   injected_quantity_in_MBq;

  friend std::ostream& operator<<(std::ostream& os, const Patient & p) {
    os << p.synfrizz_id << " " << p.name;
    return os;
  }

};
// --------------------------------------------------------------------
