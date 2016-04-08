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
#include "sydUpdateRadionuclideFilter.h"

// --------------------------------------------------------------------
template<>
void syd::Table<syd::Image>::Sort(syd::Image::vector & v, const std::string & type) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              if (a->dicoms.size() == 0) return true;
              if (b->dicoms.size() == 0) return false;
              return a->dicoms[0]->acquisition_date < b->dicoms[0]->acquisition_date;
            });
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
void syd::Table<syd::Injection>::Sort(syd::Injection::vector & v, const std::string & type) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              return a->date < b->date;
            });
}
// --------------------------------------------------------------------


// --------------------------------------------------
template<>
void syd::Table<syd::RoiMaskImage>::Sort(syd::RoiMaskImage::vector & v, const std::string & type) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              if (a->dicoms.size() == 0) return true;
              if (b->dicoms.size() == 0) return false;
              return a->dicoms[0]->acquisition_date < b->dicoms[0]->acquisition_date;
            });
}
// --------------------------------------------------


// --------------------------------------------------
template<>
void syd::Table<syd::DicomSerie>::Sort(syd::DicomSerie::vector & v, const std::string & order) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              return syd::IsDateBefore(a->acquisition_date, b->acquisition_date); });
}
// --------------------------------------------------


// --------------------------------------------------
template<>
void syd::Table<syd::Patient>::Sort(syd::Patient::vector & v, const std::string & order) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) { return a->study_id < b->study_id; });
}
// --------------------------------------------------


// --------------------------------------------------
template<>
void syd::Table<syd::Radionuclide>::Sort(syd::Radionuclide::vector & v, const std::string & order) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) { return a->atomic_number < b->atomic_number; });
}
// --------------------------------------------------


// --------------------------------------------------
template<>
void syd::Table<syd::Calibration>::Sort(syd::Calibration::vector & v, const std::string & order) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              if (a->image->dicoms.size() == 0) return true;
              if (b->image->dicoms.size() == 0) return false;
              return a->image->dicoms[0]->acquisition_date < b->image->dicoms[0]->acquisition_date;
            });
}
// --------------------------------------------------


// --------------------------------------------------
template<>
void syd::Table<syd::PixelValueUnit>::Sort(syd::PixelValueUnit::vector & v, const std::string & order) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              return a->name < b->name;
            });
}
// --------------------------------------------------


// --------------------------------------------------
template<>
void syd::Table<syd::RoiStatistic>::Sort(syd::RoiStatistic::vector & v, const std::string & order) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              if (a->image->dicoms.size() == 0) return true;
              if (b->image->dicoms.size() == 0) return false;
              return a->image->dicoms[0]->acquisition_date < b->image->dicoms[0]->acquisition_date;
            });
}
// --------------------------------------------------




// --------------------------------------------------------------------
void syd::StandardDatabase::CreateTables()
{
  //  syd::Database::CreateTables();
  AddTable<syd::RecordHistory>();
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
  AddTable<syd::Calibration>();
  AddTable<syd::PixelValueUnit>();
  AddTable<syd::RoiStatistic>();
  AddTable<syd::TimePoints>();
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
syd::Radionuclide::pointer syd::StandardDatabase::FindRadionuclide(const std::string & name) const
{
  syd::Radionuclide::pointer rad;
  odb::query<syd::Radionuclide> q = odb::query<syd::Radionuclide>::name == name;
  try {
    QueryOne(rad, q);
  } catch(std::exception & e) {
    EXCEPTION("Cannot find Radionuclide " << name << std::endl
              << "Error message is: " << e.what());
  }
  return rad;
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
  syd::GetWords(words, names);
  FindTags(tags, words);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::FindTags(syd::Tag::vector & tags, const std::vector<std::string> & names) const
{
  odb::query<Tag> q = odb::query<Tag>::label.in_range(names.begin(), names.end());
  Query<Tag>(tags, q);
  if (tags.size() != names.size()) {
    std::string s;
    for(auto t:tags) s += t->label+" ";
    std::string w;
    syd::SetWords(w, names);
    EXCEPTION("Cannot find all tags in FindTags. Look for: '" << w
              << "' but find: '" << s << "'");
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::pointer
syd::StandardDatabase::FindRoiMaskImage(const syd::Image::pointer image,
                                        const std::string & roi_name)
{
  syd::RoiType::pointer roitype = FindRoiType(roi_name);
  syd::RoiMaskImage::pointer roi;
  odb::query<syd::RoiMaskImage> q =
    odb::query<syd::RoiMaskImage>::roitype == roitype->id and
    odb::query<syd::RoiMaskImage>::frame_of_reference_uid == image->frame_of_reference_uid;
  QueryOne(roi, q);
  return roi;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::PixelValueUnit::pointer syd::StandardDatabase::FindPixelValueUnit(const std::string & name)
{
  syd::PixelValueUnit::pointer unit;
  try {
    odb::query<syd::PixelValueUnit> q = odb::query<syd::PixelValueUnit>::name == name;
    QueryOne(unit, q);
  } catch (std::exception & e) {
    EXCEPTION("Cannot find the pixelvalueunit " << name);
  }
  return unit;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::PixelValueUnit::pointer syd::StandardDatabase::FindOrInsertUnit(const std::string & name,
                                                                     const std::string & description)
{
  try {
    syd::PixelValueUnit::pointer unit = FindPixelValueUnit(name);
    return unit;
  } catch (std::exception & e) {
    syd::PixelValueUnit::pointer unit;
    New(unit);
    unit->name=name;
    unit->description=description;
    Insert(unit);
    return unit;
  }
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


// --------------------------------------------------------------------
syd::Calibration::pointer syd::StandardDatabase::FindCalibration(const syd::Image::pointer image,
                                                                 const std::string & calib_tag)
{
  syd::Tag::vector tags;
  FindTags(tags,calib_tag);
  syd::Calibration::vector calibrations;
  syd::Calibration::pointer calibration;
  typedef odb::query<syd::Calibration> QT;
  QT q = QT::image == image->id;
  Query(calibrations, q);
  int n=0;
  for(auto c:calibrations) {
    if (syd::IsAllTagsIn(c->tags, tags)) {
      if (n>0) {
        EXCEPTION("Several calibrations are associated with this image. "
                  << "I dont know which one to choose. "
                  << "Use tags to discriminate");
      }
      ++n;
      calibration = c;
    }
  }
  if (calibrations.size() < 1) {
    EXCEPTION("Cannot find calibration for this image: " << image);
  }
  return calibration;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::QueryByTag(generic_record_vector & records,
                                       const std::string table_name,
                                       const std::vector<std::string> & tag_names) // need patient name ?
{
  std::string t = syd::ToLowerCase(table_name);

  if (t == "image") return QueryByTags<syd::Image>(records, tag_names);
  //  if (table_name == "Calibration") return QueryByTag<syd::RoiStatistic>(records, tag_names);

  if (t == "roistatistic") {
    // Specific case here, we search in the image associated with the RoiStatistic
    syd::Record::vector images;
    QueryByTag(images, "Image", tag_names);
    std::vector<syd::IdType> ids;
    for(auto image:images) ids.push_back(image->id);
    syd::RoiStatistic::vector stats;
    typedef odb::query<syd::RoiStatistic> Q;
    Q q = Q::image.in_range(ids.begin(), ids.end());
    Query<syd::RoiStatistic>(stats, q);
    for(auto s:stats) records.push_back(s);
    return;
  }

  EXCEPTION("Query by tag could only be used with table that contains tags");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::InsertDefaultRecords(const std::string & def)
{
  if (def == "none") return;

  // Add some radionuclide
  LOG(1) << "Adding some default radionuclides ...";
  std::vector<std::string> rads = {"C-11", "O-15", "F-18", "P-32", "P-33",
                                   "Cu-64", "Cu-67", "Ga-68", "Sr-89",
                                   "Y-90", "Zr-89", "Tc-99m", "Rh-103m",
                                   "In-111", "Sn-117m", "I-123", "I-124",
                                   "I-125", "I-131", "Sm-153", "Ho-166",
                                   "Er-169", "Lu-177", "Re-186", "Re-188",
                                   "Tl-201", "Pb-212", "Bi-212", "Bi-213",
                                   "At-211", "Ra-223", "Ac-225", "Th-227"};
  // "Rb-82" ?
  // create  rad
  syd::Radionuclide::vector radionuclides;
  syd::UpdateRadionuclideFilter f(this);
  for(auto rad_name:rads) {
    syd::Radionuclide::pointer r;
    New(r);
    r->name = rad_name;
    f.GetDataFromWeb(r);
    radionuclides.push_back(r);
  }
  Insert(radionuclides);
  LOG(1) << radionuclides.size() << " radionuclides have been added.";

  // Add some default tags
  syd::Tag::vector tags;
  tags.push_back(NewTag("ct", "CT image"));
  tags.push_back(NewTag("spect", "SPECT image"));
  tags.push_back(NewTag("pet", "PET image"));
  tags.push_back(NewTag("init", "Initial data"));
  tags.push_back(NewTag("register", "Registered and warped image"));
  tags.push_back(NewTag("stitch", "Image computed by stitching 2 images"));
  tags.push_back(NewTag("dose", "Dose distribution image"));
  tags.push_back(NewTag("edep", "Edep distribution image"));
  tags.push_back(NewTag("dose_squared", "Squared dose distribution image (for MC simulations)"));
  tags.push_back(NewTag("edep_squared", "Squared edep distribution image (for MC simulations)"));
  tags.push_back(NewTag("dose_uncertainty", "Dose relative uncertainty distribution image (for MC simulations)"));
  tags.push_back(NewTag("edep_uncertainty", "Edep relative uncertainty distribution image (for MC simulations)"));
  tags.push_back(NewTag("spect_dc", "Decay corrected activity image"));
  tags.push_back(NewTag("activity", "Activity image (calibrated)"));
  tags.push_back(NewTag("S-matrix", "S-matrix image"));
  tags.push_back(NewTag("dose_rate", "Dose rate image"));
  tags.push_back(NewTag("tia", "Time Integrated Activity"));
  for(auto r:radionuclides) {
    tags.push_back(NewTag(r->name, "Radionuclide " + r->name));
  }
  Insert(tags);
  LOG(1) << tags.size() << " tags have been added.";

  // Add some default PixelValueUnit
  syd::PixelValueUnit::vector units;
  units.push_back(NewPixelValueUnit("%", "Percentage (such as relative uncertainty)"));
  units.push_back(NewPixelValueUnit("HU", "Hounsfield Units"));
  units.push_back(NewPixelValueUnit("counts", "Number of counts (by pixel)"));
  units.push_back(NewPixelValueUnit("label", "Mask image label"));
  units.push_back(NewPixelValueUnit("Gy", "Absorbed dose in Gy"));
  units.push_back(NewPixelValueUnit("cGy", "Absorbed dose in cGy")); // FIXME
  units.push_back(NewPixelValueUnit("MeV", "Deposited energy in MeV"));

  units.push_back(NewPixelValueUnit("Bq.h_by_IA", "Time integrated Bq (Bq.h) by injected activity in MBq"));
  units.push_back(NewPixelValueUnit("Bq_by_IA", "Activity in Bq by injected activity in MBq"));
  units.push_back(NewPixelValueUnit("MBq.h/IA[MBq]", "time integrated activity MBq.h by injected activity"));
  units.push_back(NewPixelValueUnit("MBq/IA[MBq]", "Activity in MBq by injected activity in MBq"));

  units.push_back(NewPixelValueUnit("cGy/IA[MBq]", "Dose in cGy by injected activity"));
  units.push_back(NewPixelValueUnit("cGy/h/IA[MBq]", "Dose rate in cGy by hour by injected activity"));
  units.push_back(NewPixelValueUnit("cGy/kBq.h/IA[MBq]", "Dose in cGy by tia kBq.h by injected activity (for S matrix)"));
  units.push_back(NewPixelValueUnit("kBq.h/IA[MBq]", "time integrated activity kBq.h by injected activity"));
  units.push_back(NewPixelValueUnit("kBq/IA[MBq]", "Activity in kBq by injected activity in MBq"));
  units.push_back(NewPixelValueUnit("kBq_by_IA", "Activity in kBq by injected activity in MBq"));
  units.push_back(NewPixelValueUnit("mGy/Bq.sec", "Dose by cumulated activity"));

  Insert(units);
  LOG(1) << units.size() << " PixelValueUnit have been added.";

  // Add some RoiType
  syd::RoiType::vector rois;
  rois.push_back(NewRoiType("body", "Contour of the patient"));
  rois.push_back(NewRoiType("liver", "Contour of the liver"));
  rois.push_back(NewRoiType("spleen", "Contour of the spleen"));
  rois.push_back(NewRoiType("heart", "Contour of the cardiac region"));
  rois.push_back(NewRoiType("left_kidney", "Contour of the left kidney"));
  rois.push_back(NewRoiType("right_kidney", "Contour of the right kidney"));
  rois.push_back(NewRoiType("left_lung", "Contour of the left lung"));
  rois.push_back(NewRoiType("right_lung", "Contour of the right lung"));
  rois.push_back(NewRoiType("lung", "Contour of both lungs"));
  rois.push_back(NewRoiType("bone_marrow", "Contour of the L2-L4 lumbar vertebrae"));
  rois.push_back(NewRoiType("lesion", "Lesion"));
  for(auto i=1; i<30; i++) { // start at 1
    std::ostringstream oss;
    if (i < 10) oss << "lesion0" << i;
    else oss << "lesion" << i;
    rois.push_back(NewRoiType(oss.str(), "Lesion "+ToString(i)));
  }
  Insert(rois);
  LOG(1) << rois.size() << " RoiType have been added.";

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Tag::pointer
syd::StandardDatabase::NewTag(const std::string & name,
                              const std::string & description)
{
  syd::Tag::pointer tag;
  New(tag);
  tag->label = name;
  tag->description = description;
  return tag;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::PixelValueUnit::pointer
syd::StandardDatabase::NewPixelValueUnit(const std::string & name,
                                         const std::string & description)
{
  syd::PixelValueUnit::pointer v;
  New(v);
  v->name = name;
  v->description = description;
  return v;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiType::pointer
syd::StandardDatabase::NewRoiType(const std::string & name,
                                  const std::string & description)
{
  syd::RoiType::pointer v;
  New(v);
  v->name = name;
  v->description = description;
  return v;
}
// --------------------------------------------------------------------
