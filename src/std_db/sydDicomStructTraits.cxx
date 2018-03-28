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
#include "sydDicomStructTraits.h"
#include "sydTagHelper.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(DicomStruct);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
void
syd::RecordTraits<syd::DicomStruct>::
BuildFields(const syd::Database * db) const
{
  InitCommonFields();

  /*
    Duplicate in DicomSerie. Not clear how to do.
  */

  ADD_TABLE_FIELD(patient, syd::Patient);
  AddField<std::string>("dicom_files", syd::DicomBase::GetDicomFileFunction());

  ADD_FIELD(dicom_study_uid, std::string);
  ADD_FIELD(dicom_study_name, std::string);
  ADD_FIELD(dicom_series_uid, std::string);
  ADD_FIELD(dicom_frame_of_reference_uid, std::string);
  ADD_FIELD(dicom_modality, std::string);
  ADD_FIELD(dicom_description, std::string);
  ADD_FIELD(dicom_series_description, std::string);
  ADD_FIELD(dicom_study_description, std::string);
  ADD_FIELD(dicom_image_id, std::string);
  ADD_FIELD(dicom_dataset_name, std::string);
  ADD_FIELD(dicom_manufacturer, std::string);
  ADD_FIELD(dicom_manufacturer_model_name, std::string);
  ADD_FIELD(dicom_study_id, std::string);
  ADD_FIELD(dicom_software_version, std::string);
  ADD_FIELD(dicom_patient_name, std::string);
  ADD_FIELD(dicom_patient_id, std::string);
  ADD_FIELD(dicom_patient_birth_date, std::string);
  ADD_FIELD(dicom_patient_sex, std::string);

  ADD_FIELD(dicom_structure_set_date, std::string);
  ADD_FIELD(dicom_structure_set_label, std::string);
  ADD_FIELD(dicom_structure_set_name, std::string);
  ADD_FIELD(dicom_station_name, std::string);
  ADD_FIELD(dicom_protocol_name, std::string);

  // Complete file path -> first image only
  auto f_fp = [](pointer p) -> std::string {
    if (p->dicom_files.size() == 0) return empty_value;
    return p->dicom_files[0]->GetAbsolutePath(); };
  AddField<std::string>("filepath", f_fp, "file");

  // tags
  auto f_t = [](pointer p) -> std::string { return syd::GetLabels(p->tags); };
  AddField<std::string>("tags", f_t);

  // comments
  auto f_c = [](pointer p) -> std::string { return p->GetAllComments(); };
  AddField<std::string>("comments", f_c, "com");

  // Number of rois
  auto f_nr = [](pointer p) -> int { return p->dicom_roi_names.size(); };
  AddField<int>("dicom_roi_nb", f_nr, "roi");

  // List of rois
  auto f_lr = [](pointer p) -> std::string { //TODO to put in DicomStruct 
    std::ostringstream oss;
    for(auto r:p->dicom_roi_names) oss << r << " ";
    auto s = oss.str();
    return trim(s); };
  AddField<std::string>("dicom_roi_names", f_lr, "rois");

  // Format lists
  field_format_map_["default"] =
    "id patient.name[pat] dicom_structure_set_date[date] dicom_modality[mod] dicom_roi_nb[roi] dicom_roi_names[rois] tags comments[com]";
  field_format_map_["all"] =
    "id patient.name[pat] dicom_structure_set_date[date] dicom_modality[mod] dicom_roi_nb[roi] dicom_roi_names[rois] dicom_study_uid dicom_study_name dicom_series_uid dicom_frame_of_reference_uid dicom_description dicom_series_description dicom_study_description dicom_image_id dicom_dataset_name dicom_manufacturer dicom_manufacturer_model_name dicom_study_id dicom_software_version dicom_patient_name dicom_patient_id dicom_patient_birth_date dicom_patient_sex dicom_structure_set_label dicom_structure_set_name dicom_station_name dicom_protocol_name filepath tags comments[com]";
}
// --------------------------------------------------------------------

