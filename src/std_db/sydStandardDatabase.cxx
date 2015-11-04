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
syd::PixelValueUnit::pointer syd::StandardDatabase::FindOrInsertUnit(const std::string & name,
                                                                     const std::string & description)
{
  syd::PixelValueUnit::pointer unit;
  try {
    odb::query<syd::PixelValueUnit> q = odb::query<syd::PixelValueUnit>::name == name;
    QueryOne(unit, q);
  } catch (std::exception & e) {
    New(unit);
    unit->name=name;
    unit->description=description;
    Insert(unit);
  }
  return unit;
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
syd::File::pointer syd::StandardDatabase::InsertNewFile(std::string input_path,
                                                        std::string filename,
                                                        std::string path,
                                                        bool copy)
{
  syd::File::pointer file;
  New(file);
  file->filename = filename;
  file->path = path;
  fs::path p = GetAbsolutePath(file);
  if (fs::exists(p)) {
    EXCEPTION("File '" << p << "' already exists (during InsertNewFile)");
  }
  if (copy) fs::copy_file(input_path, p);
  Insert(file);
  return file;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::RenameFile(syd::File::pointer file,
                                       std::string relative_path,
                                       std::string filename)
{
  // Check directory or create it
  CheckOrCreateRelativePath(relative_path);

  // Compute paths
  std::string new_absolute_path = ConvertToAbsolutePath(relative_path+PATH_SEPARATOR+filename);
  std::string old_absolute_path = GetAbsolutePath(file);

  // Destination exist ?
  fs::path dir(new_absolute_path);
  if (fs::exists(dir)) {
    EXCEPTION("Could not update file '"
              << old_absolute_path
              << "', destination already exists: '"
              << new_absolute_path << "'.");
  }

  // If file already exist, mv it. Do nothing if does no exist.
  fs::path old(old_absolute_path);
  if (fs::exists(old)) {
    fs::rename(old_absolute_path, new_absolute_path);
  }

  // Update in the db
  file->path = relative_path;
  file->filename = filename;
  Update(file);
}
// --------------------------------------------------
