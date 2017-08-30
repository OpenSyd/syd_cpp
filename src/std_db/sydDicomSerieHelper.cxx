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
#include "sydDicomSerieHelper.h"
#include "edlib.h"

// itk
#include "gdcmUIDGenerator.h"
#include "itkNumericSeriesFileNames.h"
#include "itkImageSeriesWriter.h"

// std
#include <functional>

// --------------------------------------------------------------------
void syd::WriteDicomToMhd(syd::DicomSerie::pointer dicom,
                          std::string user_pixel_type,
                          std::string mhd_filename)

{
  std::string pixel_type = user_pixel_type;
  if (pixel_type == "auto") {
    // read dicom header with itk header (first file only)
    if (dicom->dicom_files.size() == 0) {
      EXCEPTION("Cannot find associated file to this dicom: "
                << dicom << ". WriteDicomToMhd is aborted.");
    }
    auto header = syd::ReadImageHeader(dicom->dicom_files[0]->GetAbsolutePath());
    pixel_type =
      itk::ImageIOBase::GetComponentTypeAsString(header->GetComponentType());
  }

  // Create a list of pixel_type <-> function
  typedef std::function<void (syd::DicomSerie::pointer, std::string)> FctType;
  std::map<std::string, FctType> map = {
    { "float",  syd::WriteDicomToMhd<itk::Image<float, 3>> },
    { "short",  syd::WriteDicomToMhd<itk::Image<short, 3>> }
  };

  // Looks for the correct pixe_type and use the the corresponding function
  GetFctByPixelType<FctType>(map, pixel_type)(dicom, mhd_filename);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::CopyDictionary(itk::MetaDataDictionary &fromDict, itk::MetaDataDictionary &toDict)
{
  typedef itk::MetaDataDictionary DictionaryType;
  DictionaryType::ConstIterator itr = fromDict.Begin();
  DictionaryType::ConstIterator end = fromDict.End();
  typedef itk::MetaDataObject< std::string > MetaDataStringType;
  while (itr != end) {
    itk::MetaDataObjectBase::Pointer  entry = itr->second;
    MetaDataStringType::Pointer entryvalue =
      dynamic_cast<MetaDataStringType *>(entry.GetPointer()) ;
    if (entryvalue) {
      std::string tagkey   = itr->first;
      std::string tagvalue = entryvalue->GetMetaDataObjectValue();
      itk::EncapsulateMetaData<std::string>(toDict, tagkey, tagvalue);
    }
    ++itr;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DicomSerie::pointer
syd::InsertAnonymizedDicomSerie(const syd::DicomSerie::pointer dicom)
{
  // Get the db
  auto db = dicom->GetDatabase();

  // FIXME --> put itk/gdcm related code into sydDicomUtils, not here

  // Largely inspired from
  // https://itk.org/Doxygen/html/WikiExamples_2DICOM_2ResampleDICOM_8cxx-example.html

  // type (not really needed here -> will write pixel type as input)
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typename ImageType::Pointer output;

  // 1 read the input series
  auto dicomIO = itk::GDCMImageIO::New();
  dicomIO->SetLoadPrivateTags(true);
  dicomIO->SetLoadSequences(true);
  typedef itk::GDCMSeriesFileNames InputNamesGeneratorType;
  InputNamesGeneratorType::Pointer nameGenerator = InputNamesGeneratorType::New();
  std::string folder = db->ConvertToAbsolutePath(dicom->dicom_files[0]->path);
  nameGenerator->SetInputDirectory(folder);
  auto & temp = nameGenerator->GetFileNames(dicom->dicom_series_uid);
  typedef itk::ImageSeriesReader<ImageType> ReaderType;
  auto reader = ReaderType::New();
  reader->SetImageIO( dicomIO );
  reader->SetFileNames(temp);
  try {
    reader->Update();
  } catch (itk::ExceptionObject &excp)     {
    EXCEPTION("Exception thrown while reading the series" << std::endl
              << excp << std::endl);

  }

  // Get the pointer to the itk image
  auto itk_image = reader->GetOutput();

  // Create a MetaDataDictionary for each slice.
  ReaderType::DictionaryRawPointer inputDict = (*(reader->GetMetaDataDictionaryArray()))[0];
  ReaderType::DictionaryArrayType outputArray;

  // To keep the new series in the same study as the original we need
  // to keep the same study UID. But we need new series and frame of
  // reference UID's.
  gdcm::UIDGenerator suid;
  std::string seriesUID = suid.Generate();
  gdcm::UIDGenerator fuid;
  std::string frameOfReferenceUID = fuid.Generate();
  std::string studyUID;
  std::string sopClassUID;
  itk::ExposeMetaData<std::string>(*inputDict, "0020|000d", studyUID);
  itk::ExposeMetaData<std::string>(*inputDict, "0008|0016", sopClassUID);
  dicomIO->KeepOriginalUIDOn();

  // Nb of slices
  auto nb_slices = itk_image->GetLargestPossibleRegion().GetSize()[2];
  std::string sopInstanceUID;
  for (auto f=0; f<nb_slices; f++) {
    // Create a new dictionary for this slice
    ReaderType::DictionaryRawPointer dict = new ReaderType::DictionaryType;
    // Copy the dictionary from the first slice
    syd::CopyDictionary (*inputDict, *dict);


    // Set the UID's for the study, series, SOP  and frame of reference
    itk::EncapsulateMetaData<std::string>(*dict,"0020|000d", studyUID);
    itk::EncapsulateMetaData<std::string>(*dict,"0020|000e", seriesUID);
    itk::EncapsulateMetaData<std::string>(*dict,"0020|0052", frameOfReferenceUID);
    gdcm::UIDGenerator sopuid;
    sopInstanceUID = sopuid.Generate();
    itk::EncapsulateMetaData<std::string>(*dict,"0008|0018", sopInstanceUID);
    itk::EncapsulateMetaData<std::string>(*dict,"0002|0003", sopInstanceUID);

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // VERY VERY VERY simple anonymization
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // Patient name
    itk::EncapsulateMetaData<std::string>(*dict,"0010|0010", "ANONYMOUS");

    // Patient ID
    itk::EncapsulateMetaData<std::string>(*dict,"0010|0020", "00000000");

    // Patient's Birth Date
    itk::EncapsulateMetaData<std::string>(*dict,"0010|0030", "01012000");

    // (0010|0040) Patient's Sex
    // (0010|1000) Other Patient IDs
    // (0010|1010) Patient's Age
    // (0010|1020) Patient's Size
    // (0010|1030) Patient's Weight
    // (0010|2160) Ethnic Group
    // (0010|21b0) Additional Patient History

    // Image Position Patient: This is calculated by computing the
    // physical coordinate of the first pixel in each slice.
    ImageType::PointType position;
    ImageType::IndexType index;
    index[0] = 0;
    index[1] = 0;
    index[2] = f;
    itk_image->TransformIndexToPhysicalPoint(index, position);
    std::ostringstream value;
    value.str("");
    value << position[0] << "\\" << position[1] << "\\" << position[2];
    itk::EncapsulateMetaData<std::string>(*dict,"0020|0032", value.str());


    /*
    // Change fields that are slice specific
    std::ostringstream value;
    value.str("");
    value << f + 1;
    // Image Number
    itk::EncapsulateMetaData<std::string>(*dict,"0020|0013", value.str());
    // Series Description - Append new description to current series
    // description
    std::string oldSeriesDesc;
    itk::ExposeMetaData<std::string>(*inputDict, "0008|103e", oldSeriesDesc);
    value.str("");
    value << " DAVID " << oldSeriesDesc;
    // This is an long string and there is a 64 character limit in the
    // standard
    unsigned lengthDesc = value.str().length();
    std::string seriesDesc(value.str(), 0,
    lengthDesc > 64 ? 64
    : lengthDesc);
    itk::EncapsulateMetaData<std::string>(*dict,"0008|103e", seriesDesc);
    // Series Number
    value.str("");
    value << 1001;
    itk::EncapsulateMetaData<std::string>(*dict,"0020|0011", value.str());
    // Derivation Description - How this image was derived
    value.str("");
    value << "anonymize ";
    lengthDesc = value.str().length();
    std::string derivationDesc( value.str(), 0,
    lengthDesc > 1024 ? 1024
    : lengthDesc);
    itk::EncapsulateMetaData<std::string>(*dict,"0008|2111", derivationDesc);
    // Slice Location: For now, we store the z component of the Image
    // Position Patient.
    value.str("");
    value << position[2];
    itk::EncapsulateMetaData<std::string>(*dict,"0020|1041", value.str());

    */

    // Save the dictionary
    outputArray.push_back(dict);
  }

  // Create a new dicom serie
  auto new_dicom = db->New<syd::DicomSerie>();
  new_dicom->patient = dicom->patient;
  new_dicom->dicom_acquisition_date = dicom->dicom_acquisition_date;
  new_dicom->dicom_reconstruction_date = dicom->dicom_reconstruction_date;
  new_dicom->dicom_study_uid = studyUID;
  new_dicom->dicom_series_uid = seriesUID;
  new_dicom->dicom_frame_of_reference_uid = frameOfReferenceUID;
  new_dicom->dicom_modality = dicom->dicom_modality;
  new_dicom->dicom_description = dicom->dicom_description;
  db->Insert(new_dicom);

  auto dicom_file = db->New<syd::DicomFile>();
  dicom_file->dicom_sop_uid = sopInstanceUID; // last one
  dicom_file->dicom_instance_number = 1;
  dicom_file->path = dicom->dicom_files[0]->path;
  dicom_file->filename = "dcm_"+std::to_string(new_dicom->id)
    +"_"+sopInstanceUID+".dcm";

  // 5) Write the new DICOM series (need the id of the new dicom serie)
  typedef itk::ImageSeriesWriter<ImageType, ImageType> SeriesWriterType;
  SeriesWriterType::Pointer seriesWriter = SeriesWriterType::New();
  seriesWriter->SetInput(itk_image);
  seriesWriter->SetImageIO(dicomIO);
  std::vector<std::string> f;
  f.push_back(dicom_file->GetAbsolutePath());
  seriesWriter->SetFileNames(f);
  seriesWriter->SetMetaDataDictionaryArray(&outputArray);
  try {
    seriesWriter->Update();
  } catch (itk::ExceptionObject & excp) {
    EXCEPTION("Exception thrown while writing the series "
              << std::endl << excp << std::endl);
  }

  // Final
  db->Insert(dicom_file);
  new_dicom->dicom_files.push_back(dicom_file);
  db->Update(new_dicom);
  return new_dicom;
}
// --------------------------------------------------------------------


//--------------------------------------------------------------------
std::string syd::GetDateOfOlderDicom(const syd::DicomSerie::vector & dicoms)
{
  auto d =
    std::min_element(begin(dicoms), end(dicoms),
                     [] (const syd::DicomSerie::pointer & s1, const syd::DicomSerie::pointer & s2) {
                       return s1->dicom_acquisition_date < s2->dicom_acquisition_date;
                     });
  return (*d)->dicom_acquisition_date;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool syd::IsDicomStitchable(const syd::DicomSerie::pointer a,
                            const syd::DicomSerie::pointer b,
                            double max_reconstruction_delay)
{
  // Must be same frame_of_reference_uid and modality
  if (a->dicom_frame_of_reference_uid != b->dicom_frame_of_reference_uid) return false;
  if (a->dicom_modality != b->dicom_modality) return false;

  // if same series_uid, it is ok
  if (a->dicom_series_uid == b->dicom_series_uid) return true;

  // Cannot be acquired at the same time
  if (a->dicom_acquisition_date == b->dicom_acquisition_date) return false;
  // tentative with reconstruction_date
  // if reconstruction date difference larger than x hour, probably not the same
  double delay = syd::DateDifferenceInHours(a->dicom_reconstruction_date, b->dicom_reconstruction_date);
  if (delay > max_reconstruction_delay) return false;

  // Trial with description -> if too different, we consider it is not stichable
  auto s1 = a->dicom_description;
  auto s2 = b->dicom_description;
  EdlibAlignResult result = edlibAlign(s1.c_str(), s1.size(),
                                       s2.c_str(), s2.size(),
                                       edlibDefaultAlignConfig());
  int max = 10;
  if (result.editDistance > max) return false;

  // default
  return true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::vector<syd::DicomSerie::vector> syd::GroupByStitchableDicom(syd::DicomSerie::vector dicoms)
{
  std::vector<syd::DicomSerie::vector> final_dicoms;
  std::list<syd::DicomSerie::pointer> list;
  std::copy(dicoms.begin(), dicoms.end(), std::back_inserter(list));

  // The algorithm is:
  // 1. loop on the list of dicoms
  // 2. Search in the remaning part of the list, if another dicom is stitchable.
  // 3. If yes, group together, and remove both dicom from the list
  // ONLY valid for 2 stitched dicom max !
  // Use list instead of vector (should be faster to remove). 

  while (!list.empty()) {
    auto dicom1 = list.front();
    list.pop_front();
    syd::DicomSerie::vector a;
    a.push_back(dicom1);
    if (dicom1->dicom_modality != "CT") {
      for(auto i = list.begin(); i != list.end();) {
        // Check if stichable with all dicoms in the stitch group
        bool stichable = true;
        for(auto d:a) if (!IsDicomStitchable(d, *i)) stichable = false;
        if (stichable) {
          a.push_back(*i);
          i = list.erase(i);
        }
        else ++i;
      }
    }
    final_dicoms.push_back(a);
  }

  // Sort by date
  std::sort(begin(final_dicoms), end(final_dicoms),
            [](const syd::DicomSerie::vector & a,
               const syd::DicomSerie::vector & b) -> bool {
              auto first_date_a = syd::GetDateOfOlderDicom(a);
              auto first_date_b = syd::GetDateOfOlderDicom(b);
              return IsDateBefore(first_date_a, first_date_b);
            });

  // Remove if duplicate
  final_dicoms.erase(unique(final_dicoms.begin(), final_dicoms.end()), final_dicoms.end());
  return final_dicoms;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::CheckAndSetPatient(syd::DicomBase::pointer dicom,
                             syd::Patient::pointer patient)
{
  dicom->patient = patient;
  for(auto d:patient->dicom_patient_ids)
    if (d == dicom->dicom_patient_id) return;
  std::ostringstream ss;
  for(auto d:patient->dicom_patient_ids) ss << d << " ";
  LOG(WARNING) << "Different dicom_patient_id in db and in the Dicom file " << std::endl
               << "Patient    patient dicom ids : " << ss.str() << std::endl
               << "Dicom file patient dicom id  : " << dicom->dicom_patient_id;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
syd::Patient::pointer syd::FindPatientFromDicomInfo(syd::StandardDatabase * db, syd::DicomBase::pointer dicom)
{
  // Search for a patient with the same dicom_patient_id (brute force search)
  syd::Patient::vector patients;
  db->Query(patients);
  for(auto p:patients)
    for(auto d:p->dicom_patient_ids)
      if (d == dicom->dicom_patient_id) return p;
  return nullptr;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
syd::Patient::pointer syd::NewPatientFromDicomInfo(syd::StandardDatabase * db, syd::DicomBase::pointer dicom)
{
  // Create a patient
  auto patient = db->New<syd::Patient>();
  syd::SetPatientInfoFromDicom(dicom, patient);
  // Look for max study_id value
  syd::Patient::vector patients;
  db->Query(patients);
  int max = 0;
  for(auto p:patients) if (p->study_id > max) max = p->study_id;
  patient->study_id = max+1;
  dicom->patient = patient;
  return patient;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::SetPatientInfoFromDicom(const syd::DicomBase::pointer dicom,
                                  syd::Patient::pointer patient)
{
  patient->dicom_patient_ids.push_back(dicom->dicom_patient_id);
  // Eliminate duplicate ids (1) sort (2) unique (3) remove last indeterminate values
  std::sort(patient->dicom_patient_ids.begin(), patient->dicom_patient_ids.end());
  auto last = std::unique(patient->dicom_patient_ids.begin(), patient->dicom_patient_ids.end());
  patient->dicom_patient_ids.erase(last, patient->dicom_patient_ids.end());
  patient->name = dicom->dicom_patient_name;
  patient->sex = dicom->dicom_patient_sex;
}
//--------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::CreateDicomFolder(const syd::StandardDatabase * db,
                            const syd::DicomBase::pointer dicom)
{
  std::string relative_folder = dicom->ComputeRelativeFolder();
  std::string absolute_folder = db->ConvertToAbsolutePath(relative_folder);
  if (!fs::exists(absolute_folder)) fs::create_directories(absolute_folder);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::CopyFileToDicomFile(const std::string & filename,
                              const syd::DicomFile::pointer dicom_file,
                              int log_level,
                              bool ignore_if_exist)
{
  auto destination = dicom_file->GetAbsolutePath();
  if (ignore_if_exist and fs::exists(destination)) {
    LOG(log_level) << "Destination file already exist, ignoring";
    return false;
  }
  fs::copy_file(filename.c_str(), destination);
  LOG(log_level) << "Copying " << filename << " to " << dicom_file->path << std::endl;
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SetDicomFilePathAndFilename(syd::DicomFile::pointer file,
                                      const std::string & filename,
                                      const syd::DicomSerie::pointer & serie)
{
  if (file->id == 0) {
    LOG(FATAL) << "Cannot SetDicomFilePathAndFilename, DicomFile is not persistant: " << file;
  }

  // check if the prefix already exist
  auto f = GetFilenameFromPath(filename);
  if (f.substr(0, 4) == "dcm_") {
    f = f.substr(4, f.size()); // remove dcm_
    f = f.substr(f.find("_")+1, f.size()); // remove id_
  }

  // Build the filename
  std::ostringstream oss;
  oss << "dcm_" << file->id << "_" << f;
  file->filename = oss.str();

  // Build the relative folder
  file->path = serie->ComputeRelativeFolder();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SetDicomFilePathAndFilename(syd::DicomFile::pointer file,
                                      const std::string & filename,
                                      const syd::DicomStruct::pointer & dicom_struct)
{
  if (file->id == 0) {
    LOG(FATAL) << "Cannot SetDicomFilePathAndFilename, DicomFile is not persistant: " << file;
  }

  // check if the prefix already exist
  auto f = GetFilenameFromPath(filename);
  if (f.substr(0, 10) == "dcm_struct") {
    f = f.substr(4, f.size()); // remove dcm_struct
    f = f.substr(f.find("_")+1, f.size()); // remove id_
  }

  // Build the filename
  std::ostringstream oss;
  oss << "dcm_struct_" << file->id << "_" << GetFilenameFromPath(filename);
  file->filename = oss.str();

  // Build the relative folder
  file->path = dicom_struct->ComputeRelativeFolder();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::DicomSerie::vector syd::FindDicomSeries(const syd::Patient::pointer patient)
{
  auto db = patient->GetDatabase();
  syd::DicomSerie::vector dicoms;
  typedef odb::query<syd::DicomSerie> QI;
  QI q = QI::patient == patient->id;
  db->Query(dicoms, q);
  return dicoms;
}
// --------------------------------------------------------------------
