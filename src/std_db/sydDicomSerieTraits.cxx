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
#include "sydDicomSerieTraits.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(DicomSerie);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::DicomSerie>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();

  ADD_TABLE_FIELD(patient, syd::Patient);

  // dicom_files
  auto f_dicoms = [](pointer p) -> std::string {
    if (p->dicom_files.size() == 0) return empty_value;
    if (p->dicom_files.size() < 4) { // small number of files
      std::ostringstream oss;
      for(auto d:p->dicom_files) oss << d->id << " ";
      auto s = oss.str();
      return syd::trim(s);
    }
    std::ostringstream oss; // large nub of files
    oss << p->dicom_files[0]->id << "-" << p->dicom_files.back()->id;
    auto s = oss.str();
    return syd::trim(s);
    };
  AddField<std::string>("dicom_files", f_dicoms);

  ADD_FIELD(dicom_acquisition_date, std::string);
  ADD_FIELD(dicom_reconstruction_date, std::string);
  ADD_FIELD(dicom_study_uid, std::string);
  ADD_FIELD(dicom_series_uid, std::string);
  ADD_FIELD(dicom_frame_of_reference_uid, std::string);
  ADD_FIELD(dicom_modality, std::string);
  ADD_FIELD(dicom_description, std::string);

  // comments
  auto f_c = [](pointer p) -> std::string { return p->GetAllComments(); };
  AddField<std::string>("comments", f_c, "com");

  // Format lists
  field_format_map_["default"] =
    "id patient.name[pat] dicom_acquisition_date[date] dicom_files[files] dicom_modality[mod] dicom_description[description] dicom_reconstruction_date[rec_date]";
}
// --------------------------------------------------------------------

