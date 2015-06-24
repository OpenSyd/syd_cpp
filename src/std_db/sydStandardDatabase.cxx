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


#include "sydView-odb.hxx"


// --------------------------------------------------------------------
void syd::StandardDatabase::CreateTables()
{
  AddTable<syd::Patient>();
  AddTable<syd::Radionuclide>();
  AddTable<syd::File>();
  AddTable<syd::Tag>();
  AddTable<syd::RoiType>();

  AddTable<syd::Injection>();

  AddTable<syd::DicomSerie>();
  AddTable<syd::DicomFile>();

  AddTable<syd::Image>();
  AddTable<syd::RoiMaskImage>();

  AddTable<syd::ImageTransform>();

  AddTable<syd::Timepoint>(); // FIXME ?
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetAbsoluteFolder(const DicomSerie & serie)
{
  Patient patient = QueryOne<Patient>(serie.patient->id);
  std::string f = GetAbsoluteFolder(patient);
  std::string d = serie.acquisition_date;
  //  syd::Replace(d, " ", "_");
  // remove the hour and keep y m d
  d = d.substr(0, 10);
  f = f+PATH_SEPARATOR+d;
  f = f+PATH_SEPARATOR+serie.dicom_modality;
  return f;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::CreateAbsoluteFolder(const DicomSerie & serie)
{
  Patient patient = QueryOne<Patient>(serie.patient->id);
  std::string f = GetAbsoluteFolder(patient);
  if (!syd::DirExists(f)) syd::CreateDirectory(f);
  std::string d = serie.acquisition_date;
  //  syd::Replace(d, " ", "_");
  // remove the hour and keep y m d
  d = d.substr(0, 10);
  f = f+PATH_SEPARATOR+d;
  if (!syd::DirExists(f)) syd::CreateDirectory(f);
  f = f+PATH_SEPARATOR+serie.dicom_modality;
  if (!syd::DirExists(f)) syd::CreateDirectory(f);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetAbsoluteFolder(const Patient & patient)
{
  std::string f = GetDatabaseAbsoluteFolder()+PATH_SEPARATOR+GetRelativeFolder(patient);
  return f;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetRelativeFolder(const Patient & patient)
{
  return patient.name;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetAbsolutePath(const File & file)
{
  std::string f = GetDatabaseAbsoluteFolder()+PATH_SEPARATOR+file.path;
  f = f+PATH_SEPARATOR+file.filename;
  return f;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetAbsolutePath(const Image & image)
{
  if (image.files.size() < 1) {
    LOG(FATAL) << "Error in GetAbsolutePath, no associated file to the image: " << image;
  }
  return GetAbsolutePath(*image.files[0]);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::StandardDatabase::GetAbsoluteFolder(const Image & image)
{
  std::string f = GetAbsolutePath(image);
  return GetPathFromFilename(f);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Injection syd::StandardDatabase::FindInjectionByNameOrId(const Patient & patient,
                                                              const std::string & arg)
{
  bool found = false;
  std::string s = "%"+arg+"%";
  typedef odb::query<Injection> Q;
  Q q = Q::patient->id == patient.id and
    (Q::radionuclide->name.like(s) or Q::radionuclide->id == atoi(arg.c_str()));
  std::vector<syd::Injection> injections;
  try {
    Query(q, injections);
  }
  catch(std::exception & e) {
    LOG(FATAL) << "Fatal error in QueryInjectionByNameOrId. " << e.what();
  }
  auto n = injections.size();
  if (n == 1) return injections[0]; // success
  if (n>1) {
    LOG(FATAL) << "Several injections match '" << arg
               << "' for the patient '" << patient.name << "', abort.";
  }
  LOG(FATAL) << "Could not find injection with name or id equal to '"
             << arg << "' for the patient " << patient.name << ".";
  return injections[0];// never here, to avoid warning
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::TableElementBase * syd::StandardDatabase::InsertFromArg(const std::string & table_name,
                                                             std::vector<std::string> & arg)
{
  if (table_name == syd::Patient::GetTableName()) return InsertPatient(arg);
  if (table_name == syd::Injection::GetTableName()) return InsertInjection(arg);
  //  if (table_name == syd::DicomSerie::GetTableName()) return InsertDicomSerie(arg);
  return syd::Database::InsertFromArg(table_name, arg);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Patient * syd::StandardDatabase::InsertPatient(std::vector<std::string> & arg)
{
  syd::Patient * patient = dynamic_cast<syd::Patient*>(syd::Database::InsertFromArg(syd::Patient::GetTableName(), arg));
  // Create the folder
  std::string f = GetAbsoluteFolder(*patient);
  if (!syd::DirExists(f)) syd::CreateDirectory(f);
  return patient;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Injection * syd::StandardDatabase::InsertInjection(std::vector<std::string> & arg)
{
  if (arg.size() < 4) {
    LOG(FATAL) << "Provide <patient> <radionuclide> <date> <activity_in_MBq>. "
               << std::endl
               << " <patient> can be the study_id or the name";
  }

  // Check date
  std::string date = arg[2];
  if (!syd::IsDateValid(date)) {
    LOG(FATAL) << "The date '" << date
               << "' is not valid. It must be something like '2015-04-01 10:00'";
  }

  // Get the patient and create the Injection
  Patient p;
  syd::FindPatientByNameOrStudyId(p, this, arg[0]);

  // Check other injections for this patient
  std::vector<Injection> injections;
  Query<Injection>(odb::query<Injection>::patient->id == p.id, injections);
  for(auto i:injections) {
    double d = fabs(DateDifferenceInHours(date, i.date));
    if (d<3) {
      LOG(FATAL) << "Error: another injection already exist for this patient, with a close date (lower than 3 hours)."
                 << std::endl << "Injection: " << i;
    }
  }

  // Get Radionuclide
  syd::Radionuclide radionuclide;
  radionuclide = QueryOne<syd::Radionuclide>(odb::query<Radionuclide>::name.like(arg[1]));

  // Create the new injection
  Injection * injection = new Injection;
  injection->radionuclide = std::make_shared<Radionuclide>(radionuclide);
  injection->date = date;
  injection->activity_in_MBq = atof(arg[3].c_str());
  // This is how to insert the link to the patient object
  injection->patient = std::make_shared<Patient>(p);
  syd::Database::Insert<syd::Injection>(*injection);
  return injection;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::DumpTimepoint(const std::vector<std::string> & args, std::ostream & os, bool verboseFlag)
{
  std::vector<syd::Timepoint> timepoints;
  Query(timepoints);

  for(auto t:timepoints) {
    DD(t);
    for(auto d:t.dicoms) {
      std::cout << "\t " << *d << std::endl;
    }
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::StandardDatabase::DeleteCurrentList()
{
  bool b = syd::Database::DeleteCurrentList();
  if (!b) return false; // dont delete
  for(auto f:list_of_files_to_delete_) {
    if (std::remove(f.c_str()) != 0) {
      // LOG(WARNING) << "Could not delete the file " << f;
    }
    else {
      LOG(2) << "Deleting file " << f;
    }
  }
  list_of_files_to_delete_.clear();
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Tag syd::StandardDatabase::FindOrInsertTag(const std::string & label,
                                                const std::string & description)
{
  syd::Tag tag;
  if (Count<syd::Tag>(odb::query<syd::Tag>::label == label) == 1)
    tag = QueryOne<syd::Tag>(odb::query<syd::Tag>::label == label);
  else {
    tag.label=label;
    tag.description=description;
    Insert(tag);
  }
  return tag;
}
// --------------------------------------------------------------------
