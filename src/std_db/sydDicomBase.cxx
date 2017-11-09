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
#include "sydDicomBase.h"
#include "sydStandardDatabase.h" // (needed for callbacks)
#include "sydTagHelper.h"

//DEFINE_TABLE_IMPL(DicomBase);

// --------------------------------------------------------------------
syd::DicomBase::DicomBase():
  syd::RecordWithComments(),
  syd::RecordWithTags()
{
  patient = nullptr;
  dicom_modality = empty_value;
  dicom_description = empty_value;
  dicom_frame_of_reference_uid = empty_value;
  dicom_modality = empty_value;
  dicom_description = empty_value;
  dicom_series_description = empty_value;
  dicom_study_description = empty_value;
  dicom_study_name = empty_value;
  dicom_study_id = empty_value;
  dicom_image_id = empty_value;
  dicom_dataset_name = empty_value;
  dicom_manufacturer = empty_value;
  dicom_manufacturer_model_name = empty_value;
  dicom_software_version = empty_value;
  dicom_patient_name = empty_value;
  dicom_patient_id = empty_value;
  dicom_patient_birth_date = empty_value;
  dicom_patient_sex = empty_value;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::DicomBase::ToString() const
{
  std::stringstream ss;
  ss << (patient != nullptr? patient->name:empty_value) << " "
     << dicom_files.size() << " "
     << dicom_modality << " "
     << dicom_description << " "
     << dicom_frame_of_reference_uid << " "
     << dicom_study_uid << " "
     << dicom_series_uid << " "
     << syd::GetLabels(tags) << " "
     << GetAllComments();
  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomBase::Callback(odb::callback_event event,
                              odb::database & db) const
{
  // not needed, but safer (be sure to store file modif)
  if (event == odb::callback_event::pre_update) {
    // update the files
    for(auto f:dicom_files) db.update(f);
  }

  // When a dicom is deleted, we need to delete the
  // associated DicomFiles
  if (event == odb::callback_event::pre_erase) {
    LOG(3) << "Delete " << dicom_files.size() << " DicomFile";
    for(auto d:dicom_files) db.erase(d);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::DicomBase::Callback(odb::callback_event event, odb::database & db)
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::DicomBase::Check() const
{
  syd::CheckResult r;
  for(auto d:dicom_files) r.merge(d->Check());
  return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::function<std::string(syd::DicomBase::pointer)>
syd::DicomBase::GetDicomFileFunction()
{
  auto f = [](pointer p) -> std::string {
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
  return f;
}
// --------------------------------------------------------------------
