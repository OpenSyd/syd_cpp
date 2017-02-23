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
#include "sydPatientTraits.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(Patient);
// --------------------------------------------------------------------

// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::Patient>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();
  ADD_FIELD(name, std::string);
  ADD_FIELD(study_id, syd::IdType);
  ADD_FIELD(weight_in_kg, double);
  ADD_FIELD(dicom_patientid, std::string);
  ADD_FIELD(sex, std::string);

  field_format_map_["default"] = "id study_id name weight_in_kg[weight] sex dicom_patientid[dicom_id]";
}
// --------------------------------------------------------------------


