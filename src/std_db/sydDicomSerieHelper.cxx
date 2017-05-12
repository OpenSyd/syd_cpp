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
void syd::CopyDictionary (itk::MetaDataDictionary &fromDict, itk::MetaDataDictionary &toDict)
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
  // default
  return true;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::vector<syd::DicomSerie::vector> syd::GroupByStitchableDicom(syd::DicomSerie::vector dicoms)
{
  // Search if some dicoms are stichable (brute force)
  std::vector<syd::DicomSerie::vector> final_dicoms;
  for(auto d:dicoms) { // Look for every dicom
    bool stichable = false;
    for(auto & fd:final_dicoms) { // is already stichable with a group ?
      bool ok = true;
      // We check that this dicom is stichable with all elements of the group
      for(auto & ffd:fd) {
        if (!IsDicomStitchable(ffd, d)) { ok = false; continue; }
      }
      if (ok) { // Yes stichable -> we add id to the group
        stichable = true;
        fd.push_back(d);
        continue;
      }
    }
    if (!stichable) { // no stichable -> we add a new group
      syd::DicomSerie::vector a;
      a.push_back(d);
      final_dicoms.push_back(a);
    }
  }

  // Sort by date
  std::sort(begin(final_dicoms), end(final_dicoms),
            [](const syd::DicomSerie::vector & a,
               const syd::DicomSerie::vector & b) -> bool {
              auto first_date_a = syd::GetDateOfOlderDicom(a);
              auto first_date_b = syd::GetDateOfOlderDicom(b);
              return IsDateBefore(first_date_a, first_date_b);
            });
  return final_dicoms;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::CheckAndSetPatient(syd::DicomSerie::pointer dicom,
                             syd::Patient::pointer patient)
{
  if (patient->dicom_patientid != dicom->dicom_patient_id) {
    LOG(WARNING) << "Different dicom_patient_id in db and in DicomSerie " << std::endl
                 << "Patient    patient dicom id : " << patient->dicom_patientid << std::endl
                 << "DicomSerie patient dicom id : " << dicom->dicom_patient_id;
  }
  dicom->patient = patient;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::GuessAndSetPatient(syd::DicomSerie::pointer dicom)
{
  auto db = dicom->GetDatabase();
  odb::query<syd::Patient> q =
    odb::query<syd::Patient>::dicom_patientid == dicom->dicom_patient_id;
  syd::Patient::vector patients;
  db->Query(patients, q);
  if (patients.size() > 0) {
    dicom->patient = patients[0];
    if (patients.size() > 1) {
      std::ostringstream oss;
      for(auto p:patients) oss << p << std::endl;
      LOG(WARNING) << "Warning, several patients with same dicom_patient_id: " << oss.str();
    }
    return;
  }
  // Create a patient
  auto patient = db->New<syd::Patient>();
  syd::SetPatientInfoFromDicom(dicom, patient);
  db->Query(patients);
  int max = 0;
  for(auto p:patients) if (p->study_id > max) max = p->study_id;
  patient->study_id = max+1;
  db->Insert(patient);
  dicom->patient = patient;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void syd::SetPatientInfoFromDicom(syd::DicomSerie::pointer dicom,
                                  syd::Patient::pointer patient)
{
  patient->dicom_patientid = dicom->dicom_patient_id;
  patient->name = dicom->dicom_patient_name;
  patient->sex = dicom->dicom_patient_sex;
}
//--------------------------------------------------------------------
