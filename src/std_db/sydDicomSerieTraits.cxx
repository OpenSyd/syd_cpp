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
#include "sydTagHelper.h"

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
  ADD_TABLE_FIELD(injection, syd::Injection);
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

  ADD_FIELD(dicom_acquisition_date, std::string);
  ADD_FIELD(dicom_reconstruction_date, std::string);

  auto f_size = [](pointer p) -> std::string { return syd::ArrayToString(p->dicom_size); };
  AddField<std::string>("dicom_size", f_size);
  auto f_spacing = [](pointer p) -> std::string { return syd::ArrayToString(p->dicom_spacing); };
  AddField<std::string>("dicom_spacing", f_spacing, "sp");

  ADD_FIELD_A(dicom_pixel_scale, double, "ps");
  ADD_FIELD_A(dicom_pixel_offset, double, "po");
  ADD_FIELD_A(dicom_real_world_value_slope, double, "slope");
  ADD_FIELD_A(dicom_real_world_value_intercept, double, "intercept");
  ADD_FIELD_A(dicom_window_center, double, "wc");
  ADD_FIELD_A(dicom_window_width, double, "wl");

  ADD_FIELD_A(dicom_radionuclide_name, std::string, "rad");
  ADD_FIELD_A(dicom_counts_accumulated, double, "cts");
  ADD_FIELD(dicom_actual_frame_duration_in_msec, double);
  ADD_FIELD(dicom_number_of_frames_in_rotation, int);
  ADD_FIELD(dicom_number_of_rotations, int);
  ADD_FIELD(dicom_table_traverse_in_mm, double);
  ADD_FIELD(dicom_table_height_in_mm, double);
  ADD_FIELD(dicom_rotation_angle, double);

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

  // Format lists
  field_format_map_["default"] =
    "id patient.name[pat] dicom_acquisition_date[date] dicom_modality[mod] dicom_series_description[serie] dicom_dataset_name[dsn] dicom_image_id[dii] dicom_reconstruction_date[rec_date] tags comments[com]";
  field_format_map_["desc"] =
    "id patient.name[pat] dicom_acquisition_date[date] dicom_files[files] dicom_modality[mod] dicom_description[description] dicom_reconstruction_date[rec_date] injection.id[inj] tags comments[com]";
  field_format_map_["image"] =
    "id patient.name[pat] dicom_acquisition_date[date] dicom_files[files] dicom_modality[mod] dicom_size[size] dicom_spacing[spacing] dicom_reconstruction_date[rec_date] tags comments[com]";
}
// --------------------------------------------------------------------

