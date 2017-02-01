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
#include "sydImage.h"
#include "sydStandardDatabase.h"
#include "sydDicomSerie.h"
#include "sydTagHelper.h"

DEFINE_TABLE_IMPL(Image);

// --------------------------------------------------------------------
syd::Image::Image():
  syd::Record(),
  syd::RecordWithHistory(),
  syd::RecordWithTags(),
  syd::RecordWithComments()
{
  patient = nullptr;
  injection = nullptr;
  pixel_unit = nullptr;
  modality = "image"; // default (not empty_value)
  type = pixel_type = acquisition_date = frame_of_reference_uid = empty_value;
  dimension = 0;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Image::GetPatientName() const
{
  std::string name = empty_value;
  if (patient != nullptr) name = patient->name;
  return name;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Image::GetInjectionName() const
{
  std::string inj_name = empty_value;
  if (injection != nullptr) inj_name = injection->radionuclide->name
                              +"("+std::to_string(injection->id)+")";
  return inj_name;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Image::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << GetPatientName() << " "
     << GetInjectionName() << " ";
  // (only first file is usually useful to display)
  if (files.size() != 0) ss << files[0]->filename << " ";
  else ss << empty_value << " ";
  ss << syd::GetLabels(tags) << " "
     << acquisition_date << " "
     << type << " "
     << pixel_type << " "
     << dimension << " "
     << SizeAsString() << " "
     << SpacingAsString() << " "
     << modality << " ";
  for(auto d:dicoms) ss << d->id << " ";
  if (dicoms.size() == 0) ss << empty_value << " ";
  ss << frame_of_reference_uid << " ";
  if (pixel_unit != nullptr) ss << pixel_unit->name;
  else ss << empty_value << " ";
  if (history and print_history_flag_) {
    ss << " " << history->insertion_date << " "
       << history->update_date << " ";
  }
  else ss << empty_value << " ";
  ss << GetAllComments() << std::endl;
  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Image::ToShortString() const
{
  std::stringstream ss ;
  ss << id << " "
     << GetPatientName() << " "
     << GetInjectionName() << " "
     << syd::GetLabels(tags) << " "
     << acquisition_date << " "
     << type << " "
     << pixel_type << " "
     << dimension << " "
     << SizeAsString() << " "
     << SpacingAsString() << " "
     << modality << " ";
  for(auto d:dicoms) ss << d->id << " ";
  if (dicoms.size() == 0) ss << empty_value << " ";
  if (pixel_unit != nullptr) ss << pixel_unit->name;
  else ss << empty_value << " ";
  if (history and print_history_flag_) {
    ss << " " << history->insertion_date << " "
       << history->update_date;
  }
  else ss << empty_value;
  ss << GetAllComments() << std::endl;
  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------------------------


// --------------------------------------------------
std::string syd::Image::GetAcquisitionDate() const
{
  return acquisition_date;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::AddDicomSerie(const syd::DicomSerie::pointer dicom)
{
  bool found = false;
  int i=0;
  while (i<dicoms.size() and !found) {
    if (dicoms[i]->id == dicom->id) found = true;
    ++i;
  }
  if (!found) dicoms.push_back(dicom);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::RemoveDicomSerie(const syd::DicomSerie::pointer dicom)
{
  bool found = false;
  int i=0;
  while (i<dicoms.size() and !found) {
    if (dicoms[i]->id == dicom->id) {
      found = true;
      dicoms.erase(dicoms.begin()+i);
    }
    ++i;
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::Callback(odb::callback_event event,
                          odb::database & db) const
{
  syd::Record::Callback(event,db);
  syd::RecordWithHistory::Callback(event,db, db_);

  if (event == odb::callback_event::post_erase) {
    for(auto f:files) db.erase(f);
  }
  if (event == odb::callback_event::pre_persist) {
    // insert the file with odb::database not the syd::database
    for(auto f:files) db.persist(f);
  }
  if (event == odb::callback_event::pre_update) {
    // update the files
    for(auto f:files) db.update(f);
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::Callback(odb::callback_event event,
                          odb::database & db)
{
  syd::Record::Callback(event,db);
  syd::RecordWithHistory::Callback(event,db, db_);
  if (event == odb::callback_event::post_erase) {
    for(auto f:files) db.erase(f);
  }
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::Image::IsSameSizeAndSpacingThan(const syd::Image::pointer image) const
{
  bool b = true;
  for(auto i=0; i<size.size(); i++) {
    b = b and size[i] == image->size[i] and
      spacing[i] == image->spacing[i];
  }
  return b;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::FatalIfNoDicom() const
{
  if (dicoms.size() == 0) {
    LOG(FATAL) << "Error the following image does not have associated DicomSerie."
               << std::endl << *this;
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Image::CopyDicomSeries(const syd::Image::pointer image)
{
  dicoms.clear();
  for(auto d:image->dicoms) AddDicomSerie(d);
}
// --------------------------------------------------


// --------------------------------------------------------------------
syd::CheckResult syd::Image::Check() const
{
  syd::CheckResult r;
  for(auto f:files) r.merge(f->Check());
  for(auto d:dicoms) r.merge(d->Check());
  return r;
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
double syd::Image::GetHoursFromInjection() const
{
  if (injection == nullptr) return 0.0;
  if (acquisition_date == empty_value) return 0.0;
  std::string inj_date = injection->date;
  double time = syd::DateDifferenceInHours(acquisition_date, inj_date);
  return time;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Image::GetAbsolutePath() const
{
  if (files.size() == 0) return empty_value;
  return files[0]->GetAbsolutePath();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Image::ComputeDefaultRelativePath()
{
  if (patient == nullptr) {
    EXCEPTION("Cannot compute the default image relative path"
              << ", no patient ar yet associated with the image: "
              << ToString());
  }
  auto s = patient->name;
  syd::Replace(s, " ", "_"); // replace space with underscore
  if (!fs::portable_name(s)) {
    EXCEPTION("The folder name '" << s << "' does not seems a "
              << " valid and portable dir name. (you man change "
              << "the patient name. Abort.");
  }
  return s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::Image::ComputeDefaultMHDFilename()
{
  if (!IsPersistent()) {
    EXCEPTION("Image must be persistent (in the db) to "
              << "use ComputeDefaultMHDFilename.");
  }
  std::ostringstream oss;
  oss << modality << "_" << id << ".mhd";
  return oss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/*
  void syd::Image::SetDefaultFields(std::map<std::string, syd::Record::GetFieldFunction> & map) const
  {
  DDF();
  syd::Record::SetDefaultFields(map);
  //    if pname date tag inj injq modality size spacing comments unit dicom

  // To shorten the code
  auto cast = [](syd::Record::pointer r) ->
  syd::Image::pointer { return std::static_pointer_cast<syd::Image>(r); };
  typedef syd::Record::pointer T;

  // add_field<syd::Image>(map, "pname", XXXX?? ) how todo with macro ? NO

  map["pname"] = [cast](T r) { return cast(r)->GetPatientName(); };
  map["date"] = [cast](T r) { return cast(r)->GetAcquisitionDate(); };
  map["inj"] = [cast](T r) { return cast(r)->GetInjectionName(); };
  map["injq"] = [cast](T r) {
  if (cast(r)->injection == nullptr) return syd::empty_value;
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(2) << cast(r)->injection->activity_in_MBq;
  return oss.str();
  };

  // FIXME WILL NOT WORK
  std::map<std::string, syd::Record::GetFieldFunction> inj_map;
  if (injection == nullptr) return;
  injection->SetDefaultFields(inj_map); // FIXME !!! use fake injection ?
  DD("here");
  for(auto m:inj_map) {
  std::string n= "injection->"+m.first;
  DD(n);
  map[n] = m.second;
  }

  }
*/
// --------------------------------------------------------------------

