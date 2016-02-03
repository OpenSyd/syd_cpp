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
  odb::query<Tag> q = odb::query<Tag>::label.in_range(words.begin(), words.end());
  Query<Tag>(tags, q);
  if (tags.size() != words.size()) {
    std::string s;
    for(auto t:tags) s += t->label+" ";
    EXCEPTION("Cannot find all tags in FindTags. Look for: '" << names
              << "' but find: '" << s << "'");
  }
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
namespace syd {
  TABLE_GET_NUMBER_OF_ELEMENTS(Patient)
  TABLE_GET_NUMBER_OF_ELEMENTS(Injection)
  TABLE_GET_NUMBER_OF_ELEMENTS(Radionuclide)
  TABLE_GET_NUMBER_OF_ELEMENTS(Tag)
  TABLE_GET_NUMBER_OF_ELEMENTS(File)
  TABLE_GET_NUMBER_OF_ELEMENTS(DicomFile)
  TABLE_GET_NUMBER_OF_ELEMENTS(DicomSerie)
  TABLE_GET_NUMBER_OF_ELEMENTS(Image)
  TABLE_GET_NUMBER_OF_ELEMENTS(RoiType);
  TABLE_GET_NUMBER_OF_ELEMENTS(RoiMaskImage);
  TABLE_GET_NUMBER_OF_ELEMENTS(ImageTransform);
  TABLE_GET_NUMBER_OF_ELEMENTS(Calibration);
  TABLE_GET_NUMBER_OF_ELEMENTS(PixelValueUnit);
  TABLE_GET_NUMBER_OF_ELEMENTS(RoiStatistic);
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
                                       const std::vector<std::string> & tag_names) const
{
  if (tag_names.size() == 0) {
    Query(records, table_name);
    return;
  }
  if (table_name == "Image") {
    syd::Image::vector images;
    QueryImageByTag(images, tag_names[0]);
    for(auto image:images) {
      int n=0;
      for(auto t:tag_names) { // brute force search !!
        auto iter = std::find_if(image->tags.begin(), image->tags.end(),
                                 [&t](syd::Tag::pointer & tag)->bool { return tag->label == t;} );
        if (iter == image->tags.end()) continue;
        else ++n;
      }
      if (n == tag_names.size()) records.push_back(image);
    }
    return;
  }
  EXCEPTION("Query by tag could only be used with syd::Image");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
// Specific query to match image with a tag name, in order to speed up
// a bit the FindImageByTag.
void syd::StandardDatabase::QueryImageByTag(syd::Image::vector & images,
                                            const std::string & tag_name) const
{
  std::vector<syd::IdType> ids; // resulting id of the images

  // Create request code
  std::ostringstream sql; // request
  std::string t1="\"syd::Image\"";
  std::string t2="\"syd::Image_tags\"";
  std::string t3="\"syd::Tag\"";
  sql << "select " << t1 << ".id ";
  sql << "from   " << t1 << "," << t2 << "," << t3 << " ";
  sql << "where  " << t1 << ".id == " << t2 << ".object_id ";
  sql << "and    " << t2 << ".value == " << t3 << ".id ";
  sql << "and " << t3 << ".label==" << "\"" << tag_name << "\" ";
  sql << ";";

  // Native query
  try {
  odb::sqlite::connection_ptr c (odb_db_->connection ());
  sqlite3 * sdb(c->handle());
  sqlite3_stmt * stmt;
  std::string s = sql.str();
  auto rc = sqlite3_prepare_v2(sdb, s.c_str(), -1, &stmt, NULL);
  if (rc==SQLITE_OK) {
    while(sqlite3_step(stmt) == SQLITE_ROW) {
      std::string n = syd::sqlite3_column_text_string(stmt, 0);
      ids.push_back(atoi(n.c_str()));
    }
  }
  } catch(std::exception & e) {
    EXCEPTION("Error during sql query. Error is " << e.what());
  }

  // Retrieve images
  Query(images, ids);
}
// --------------------------------------------------------------------
