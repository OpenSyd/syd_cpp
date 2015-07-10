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
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
void syd::StandardDatabase::CreateTables()
{
  AddTable<syd::Patient>();
  AddTable<syd::Injection>();
  AddTable<syd::Radionuclide>();
  AddTable<syd::Tag>();
  AddTable<syd::File>();
  AddTable<syd::DicomFile>();
  AddTable<syd::DicomSerie>();
  AddTable<syd::Image>();
  AddTable<syd::RoiType>();
  AddTable<syd::RoiMaskImage>();
  AddTable<syd::ImageTransform>();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Patient::pointer syd::StandardDatabase::FindPatient(const std::string & name_or_study_id) const
{
  syd::Patient::pointer patient;
  odb::query<syd::Patient> q =
    odb::query<syd::Patient>::name == name_or_study_id or
    odb::query<syd::Patient>::study_id == atoi(name_or_study_id.c_str());
  try {
    QueryOne(patient, q);
  } catch(std::exception & e) {
    EXCEPTION("Error in FindPatient with param: " << name_or_study_id << std::endl
              << "Error message is: " << e.what());
  }
  return patient;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Injection::pointer syd::StandardDatabase::FindInjection(const syd::Patient::pointer patient,
                                                             const std::string & name_or_study_id) const
{
  syd::Injection::pointer injection;
  odb::query<syd::Injection> q =
    odb::query<syd::Injection>::patient == patient->id and (
    odb::query<syd::Injection>::radionuclide->name == name_or_study_id.c_str() or
    odb::query<syd::Injection>::id == atoi(name_or_study_id.c_str()));

  try {
    QueryOne(injection, q);
  } catch(std::exception & e) {
    EXCEPTION("Error in FindInjection for patient " << patient->name
              << ", with param: " << name_or_study_id << std::endl
              << "Error message is: " << e.what());
  }
  return injection;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::FindTags(syd::Tag::vector & tags, const std::string & names) const
{
  std::vector<std::string> words;
  syd::GetWords(names, words);
  odb::query<Tag> q = odb::query<Tag>::label.in_range(words.begin(), words.end());
  Query<Tag>(tags, q);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Tag::pointer syd::StandardDatabase::FindOrInsertTag(const std::string & label,
                                                         const std::string & description)
{
  syd::Tag::pointer tag;
  try {
    odb::query<syd::Tag> q = odb::query<syd::Tag>::label == label;
    QueryOne(tag, q);
  } catch (std::exception & e) {
    New(tag);
    tag->label=label;
    tag->description=description;
    Insert(tag);
  }
  return tag;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiType::pointer syd::StandardDatabase::FindRoiType(const std::string & roiname) const
{
  syd::RoiType::pointer r;
  odb::query<syd::RoiType> q = odb::query<RoiType>::name == roiname;
  QueryOne(r, q);
  return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer
syd::StandardDatabase::FindRoiMaskImage(const syd::RoiType::pointer roitype,
                                        const syd::DicomSerie::pointer dicom) const
{
  syd::Patient::pointer patient = dicom->patient;

  // Get all mask for this patient and this roitype
  syd::RoiMaskImage::vector masks;
  odb::query<RoiMaskImage> q = odb::query<RoiMaskImage>::image->patient == patient->id and
    odb::query<RoiMaskImage>::roitype == roitype->id;
  Query(masks, q);

  // Select the one associated with the dicom
  bool found = false;
  syd::RoiMaskImage::vector results;
  for(auto m:masks) {
    if (m->image->dicoms.size() != 1) {
      LOG(WARNING) << "Warning the image of this mask does not have a single dicom (ignoring): " << m;
      continue;
    }
    if (dicom->dicom_frame_of_reference_uid == m->image->dicoms[0]->dicom_frame_of_reference_uid)
      results.push_back(m);
  }
  if (results.size() == 0) {
    EXCEPTION("No RoiMaskImage found for " << patient->name << " " << roitype->name
              << ", with dicom " << dicom->id << " frame_of_reference_uid = " << dicom->dicom_frame_of_reference_uid);
  }
  if (results.size() > 1) {
    std::string s;
    for(auto r:results) s += "\n"+r->ToString();
    EXCEPTION("Several RoiMaskImage found for " << patient->name << ", " << roitype->name
              << ", dicom " << dicom->id << ", frame_of_reference_uid = " << dicom->dicom_frame_of_reference_uid
              << s;);
  }
  return results[0];
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetAbsolutePath(const syd::Image::pointer image) const
{
  if (image->files.size() == 0) return "unset_file";
  else return GetAbsolutePath(image->files[0]);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetAbsolutePath(const syd::DicomFile::pointer dicom) const
{
  return GetAbsolutePath(dicom->file);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetAbsolutePath(const syd::File::pointer file) const
{
  std::string f = file->path+PATH_SEPARATOR+file->filename;
  return ConvertToAbsolutePath(f);
}
// --------------------------------------------------------------------




// --------------------------------------------------
// template<>
// void syd::StandardDatabase::Sort(std::vector<std::shared_ptr<syd::Patient>> & v, const std::string & type) const
// {
//   DD(" sort for patient");
//   typedef std::shared_ptr<syd::Patient> pointer;
//   std::sort(begin(v), end(v),
//             [v](pointer a, pointer b) { return a->study_id < b->study_id; });
// }
// --------------------------------------------------
