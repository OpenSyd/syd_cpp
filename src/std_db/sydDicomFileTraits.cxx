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
#include "sydDicomFileTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(DicomFile);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::DicomFile>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();

  ADD_FIELD(dicom_sop_uid, std::string);
  ADD_FIELD(dicom_instance_number, int);

  // Inherit File
  field_map_["filename"] = db->NewField("File", "filename");
  field_map_["path"] = db->NewField("File", "path");
  field_map_["md5"] = db->NewField("File", "md5");

  // Format lists
  field_format_map_["default"] =
    "id filename path dicom_instance_number[n] dicom_sop_uid[uid] md5";
}
// --------------------------------------------------------------------

