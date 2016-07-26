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
#include "sydDicomSerieBuilder.h"

// itk (gdcm)
#include "gdcmGlobal.h"
#include "gdcmDictEntry.h"
#include "gdcmDicts.h"

namespace syd {


  // --------------------------------------------------------------------
  std::string DicomSerieBuilder::GetStringValueFromTag(itk::GDCMImageIO::Pointer dicomIO,
                                                       const std::string & key)
  {
    std::string s = empty_value;
    dicomIO->GetValueFromTag(key, s);
    return s;
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  double DicomSerieBuilder::GetDoubleValueFromTag(itk::GDCMImageIO::Pointer dicomIO,
                                                  const std::string & key)
  {
    double v = 0.0;
    const  DictionaryType & dictionary = dicomIO->GetMetaDataDictionary();
    typedef itk::MetaDataObject< double > MetaDataDoubleType;
    DictionaryType::ConstIterator tagItr = dictionary.Find(key);
    DictionaryType::ConstIterator end = dictionary.End();
    if (tagItr != end) {
      MetaDataDoubleType::ConstPointer entryvalue =
        dynamic_cast<const MetaDataDoubleType *>(tagItr->second.GetPointer());
      if (entryvalue) {
        v = entryvalue->GetMetaDataObjectValue();
      }
    }
    return v;
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  // FIXME template ?
  unsigned short DicomSerieBuilder::GetUShortValueFromTag(itk::GDCMImageIO::Pointer dicomIO,
                                                          const std::string & key)
  {
    double v = 0.0;
    const  DictionaryType & dictionary = dicomIO->GetMetaDataDictionary();
    typedef itk::MetaDataObject< unsigned short > MetaDataUShortType;
    DictionaryType::ConstIterator tagItr = dictionary.Find(key);
    DictionaryType::ConstIterator end = dictionary.End();
    if (tagItr != end) {
      MetaDataUShortType::ConstPointer entryvalue =
        dynamic_cast<const MetaDataUShortType *>(tagItr->second.GetPointer());
      if (entryvalue) {
        v = entryvalue->GetMetaDataObjectValue();
      }
    }
    return v;
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  DicomSerieBuilder::DicomSerieBuilder(StandardDatabase * db):DicomSerieBuilder()
  {
    SetDatabase(db);
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  DicomSerieBuilder::~DicomSerieBuilder()
  {
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  DicomSerieBuilder::DicomSerieBuilder()
  {
    // Insert some tags that not always known in the dicom dictionary

    // DatasetName
    DcmDictEntry * e = new DcmDictEntry(0x0011, 0x1012, EVR_LO, "DatasetName", 0,
                                        DcmVariableVM, NULL, true, NULL);
    DcmDataDictionary &globalDataDict = dcmDataDict.wrlock();
    globalDataDict.addEntry(e);

    // InstanceNumber
    e = new DcmDictEntry(0x0020, 0x0013, EVR_IS, "InstanceNumber", 0,
                         DcmVariableVM, NULL, true, NULL);
    globalDataDict.addEntry(e);

    // NumberofFramesinRotation
    e = new DcmDictEntry(0x0054, 0x0053, EVR_US, "NumberofFramesinRotation", 0,
                         DcmVariableVM, NULL, true, NULL);
    globalDataDict.addEntry(e);

    // NumberofRotations
    e = new DcmDictEntry(0x0054, 0x0051, EVR_US, "NumberofRotations", 0,
                         DcmVariableVM, NULL, true, NULL);
    globalDataDict.addEntry(e);

    // PixelScale
    e = new DcmDictEntry(0x0011, 0x103B, EVR_FD, "PixelScale", 0,
                         DcmVariableVM, NULL, true, NULL);
    globalDataDict.addEntry(e);

    // init
    patient_ = NULL;
    db_ = NULL;
    forcePatientFlag_ = false;
    nb_of_skip_files = 0;
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  void DicomSerieBuilder::SetPatient(Patient::pointer p)
  {
    patient_ = p;
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  void DicomSerieBuilder::SearchDicomInFile(std::string filename)
  {
    // Open the file
    DcmFileFormat dfile;
    bool b = OpenDicomFile(filename, dfile);
    if (!b) {  // this is not a dicom file
      LOG(WARNING) << "Error the file '" << filename << "' is not a dicom file.";
      return;
    }
    DcmObject * dset = dfile.getAndRemoveDataset();

    // Open the Dicomfile
    // FIXME put as a functino
    typedef itk::GDCMImageIO ImageIOType;
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    dicomIO->SetFileName(filename);
    dicomIO->SetLoadPrivateTags(true);
    dicomIO->SetLoadSequences(true);
    try {
      dicomIO->ReadImageInformation();
    } catch (std::exception & e) {
      LOG(WARNING) << "Error cannot read '" << filename << "' (it is not a dicom file ?).";
      return;
    }
    DD("Dicom is open");

    // Test if this dicom file already exist in the db
    //std::string sop_uid = GetTagValueString(dset, "SOPInstanceUID");
    std::string sop_uid;
    b = dicomIO->GetValueFromTag("0008|0018", sop_uid); // SOPInstanceUID
    if (!b) {
      LOG(WARNING) << "Cannot find tag 0008|0018 SOPInstanceUID. Ignored";
      return;
    }
    DD(sop_uid);
    if (DicomFileAlreadyExist(sop_uid)) {
      LOG(2) << "Dicom file with same sop_uid already exist in the db. Skipping " << filename;
      nb_of_skip_files++;
      return;
    }

    // Test if a serie already exist in the database
    DicomSerie::pointer serie;
    b = GuessDicomSerieForThisFile(filename, dicomIO, serie);

    // If this is a new DicomSerie, we create it
    if (!b) {
      db_->New(serie);
      UpdateDicomSerie(serie, filename, dicomIO);
      series_to_insert.push_back(serie);
      LOG(2) << "Creating a new serie: " << serie->dicom_series_uid;
    }

    // Then we add this dicomfile to the serie
    DicomFile::pointer dicomfile = CreateDicomFile(filename, dicomIO, serie);
    dicomfiles_to_insert.push_back(dicomfile);
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  bool DicomSerieBuilder::DicomFileAlreadyExist(const std::string & sop_uid)
  {
    DicomFile::vector df;
    odb::query<DicomFile> q = odb::query<DicomFile>::dicom_sop_uid == sop_uid;
    db_->Query(df, q);
    int n = df.size();
    if (n>0) return true;
    for(auto f:dicomfiles_to_insert) {
      if (f->dicom_sop_uid == sop_uid) return true;
    }
    return false;
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  bool DicomSerieBuilder::GuessDicomSerieForThisFile(const std::string & filename,
                                                     itk::GDCMImageIO::Pointer dicomIO,
                                                     //DcmObject * dset,
                                                     DicomSerie::pointer & serie)
  {
    DD("GuessDicomSerieForThisFile");

    // Check in the future series
    std::string series_uid;
    bool b = dicomIO->GetValueFromTag("0020|000e", series_uid); // SeriesInstanceUID
    if (!b) {
      LOG(WARNING) << "Cannot find tag 00020|000a SeriesInstanceUID. Ignored";
      return false;
    }
    std::string modality;
    b = dicomIO->GetValueFromTag("0008|0060", modality); // Modality
    if (!b) {
      LOG(WARNING) << "Cannot find tag 0008|0060 Modality. Ignored";
      return false;
    }

    int index = -1;
    for(auto i=0; i<series_to_insert.size(); i++) {
      DicomSerie::pointer s = series_to_insert[i];
      // First check series_uid and modality
      if (s->dicom_series_uid != series_uid) continue;
      if (modality != s->dicom_modality) continue;

      // Here we found a DicomSerie with the same series_uid
      // Very simple heuristic based on the modality
      if (s->dicom_modality == "CT") {
        if (index != -1) {
          LOG(FATAL) << "Error two different CT DicomSerie with the same series_uid exist. Database corrupted."
                     << std::endl
                     << "First serie: " << series_to_insert[index] << std::endl
                     << "Second serie: " << series_to_insert[i] << std::endl;
        }
        index = i;
      }
    }
    // We find a corresponding serie
    if (index != -1) {
      serie = series_to_insert[index];
      return true;
    }

    // Find all existing DicomSerie with the same uid, in the db
    DicomSerie::vector series;
    odb::query<DicomSerie> q = odb::query<DicomSerie>::dicom_series_uid == series_uid;
    db_->Query(series, q);
    index=-1;
    for(auto i=0; i<series.size(); i++) {
      auto s = series[i];
      if (modality != s->dicom_modality) continue;
      // Here we found a DicomSerie with the same series_uid
      // Very simple heuristic based on the modality
      if (s->dicom_modality == "CT") {
        if (index != -1) {
          LOG(FATAL) << "Error two different CT DicomSerie with the same series_uid exist. Database corrupted."
                     << std::endl
                     << "First serie: " << series[index] << std::endl
                     << "Second serie: " << s << std::endl;
        }
        index = i;
      }
    }

    // We find a corresponding serie
    if (index != -1) {
      serie = series[index];
      return true;
    }

    // Nothing found
    return false;
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  /// Update the field
  void DicomSerieBuilder::UpdateDicomSerie(DicomSerie::pointer serie)
  {
    // Find associated DicomFile
    syd::DicomFile::vector files;
    typedef odb::query<syd::DicomFile> Q;
    Q q = Q::dicom_serie == serie->id;
    db_->Query(files, q);
    if (files.size() ==0) {
      EXCEPTION("No DicomFile for this DicomSerie");
    }

    // Set the current patient
    patient_ = serie->patient;

    // Open the first file
    auto filename = db_->GetAbsolutePath(files[0]);
    // Open the Dicomfile
    typedef itk::GDCMImageIO ImageIOType;
    ImageIOType::Pointer dicomIO = ImageIOType::New();
    dicomIO->SetFileName(filename);
    dicomIO->SetLoadPrivateTags(true);
    dicomIO->SetLoadSequences(true);
    try {
      dicomIO->ReadImageInformation();
    } catch (std::exception & e) {
      LOG(WARNING) << "Error cannot read '" << filename << "' (it is not a dicom file ?).";
      return;
    }
    UpdateDicomSerie(serie, filename, dicomIO);
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  /// Do not check if the serie already exist
  void DicomSerieBuilder::UpdateDicomSerie(DicomSerie::pointer serie,
                                           const std::string & filename,
                                           itk::GDCMImageIO::Pointer dicomIO)
  {
    // get date
    std::string AcquisitionTime = GetStringValueFromTag(dicomIO, "0008|0032"); // Acquisition Time
    std::string AcquisitionDate = GetStringValueFromTag(dicomIO, "0008|0022"); // Acquisition Date
    std::string ContentDate = GetStringValueFromTag(dicomIO, "0008|0023"); // Content Date
    std::string ContentTime = GetStringValueFromTag(dicomIO, "0008|0033");// Content Time
    std::string InstanceCreationDate = GetStringValueFromTag(dicomIO, "0008|0012"); // Instance Creation Date
    std::string InstanceCreationTime = GetStringValueFromTag(dicomIO, "0008|0013"); //Instance Creation Time
    std::string acquisition_date = ConvertDicomDateToStringDate(AcquisitionDate, AcquisitionTime);
    std::string reconstruction_date = ConvertDicomDateToStringDate(ContentDate, ContentTime);

    if (reconstruction_date.empty())
      reconstruction_date = ConvertDicomDateToStringDate(InstanceCreationDate, InstanceCreationTime);
    if (acquisition_date.empty())
      acquisition_date = ConvertDicomDateToStringDate(InstanceCreationDate, InstanceCreationTime);

    // Patient, injection (do not check here that injection is really associated with the patient)
    std::string patientID = GetStringValueFromTag(dicomIO, "0010|0020");   // Patient ID
    std::string patientName = GetStringValueFromTag(dicomIO, "0010|1001"); //Patient Name
    char * sex = new char[100];
    dicomIO->GetPatientSex(sex);

    if (!forcePatientFlag_) {
      LOG(3) << "Check patient dicom_patientid is the same than the given patient";
      if (patient_->dicom_patientid != patientID) {
        LOG(FATAL) << "Patient does not seems to be the same. You ask for " << patient_->name
                   << " with dicom_id = " << patient_->dicom_patientid
                   << " while in dicom, it is '" << patientID
                   << " with name: " << patientName << std::endl
                   << "Filename is " << filename << std::endl
                   << "Use 'forcePatient' if you want to bypass this check";
      }
      if (patient_->sex != std::string(sex)) {
        LOG(FATAL) << "Patient's sex is different in the db and in the dicom: "
                   << patient_->sex << " vs " << sex;
      }
    }
    if (patient_->dicom_patientid != patientID) { // update the dicom id if it is different (force flag)
      patient_->dicom_patientid = patientID;
      patient_->sex = sex;
      db_->Update<Patient>(patient_);
    }
    serie->patient = patient_;

    // Modality
    serie->dicom_modality = GetStringValueFromTag(dicomIO, "0008|0060"); //Modality

    // UID
    serie->dicom_study_uid = GetStringValueFromTag(dicomIO, "0020|000d"); //StudyInstanceUID
    serie->dicom_series_uid = GetStringValueFromTag(dicomIO, "0020|000e"); //SeriesInstanceUID
    serie->dicom_frame_of_reference_uid = GetStringValueFromTag(dicomIO, "0020|0052");//FrameOfReferenceUID
    //  what about "DatasetUID ?

    // Date
    serie->dicom_acquisition_date = acquisition_date;
    serie->dicom_reconstruction_date = reconstruction_date;

    // Description. We merge the tag because it is never consistant
    std::string SeriesDescription = GetStringValueFromTag(dicomIO, "0008|103e"); // SeriesDescription
    std::string StudyDescription = GetStringValueFromTag(dicomIO, "0008|1030"); // StudyDescription
    std::string ImageID = GetStringValueFromTag(dicomIO, "0054|0400"); // Image ID
    std::string DatasetName = GetStringValueFromTag(dicomIO, "0011|1012"); // DatasetName
    std::string description = SeriesDescription+" "+StudyDescription
      +" "+ImageID+" "+DatasetName;

    // Device
    std::string Manufacturer = GetStringValueFromTag(dicomIO, "0008|0070"); // Manufacturer
    std::string ManufacturerModelName = GetStringValueFromTag(dicomIO, "0008|1090"); //ManufacturerModelName
    DD(ManufacturerModelName);
    DD(Manufacturer);
    description = description + " " + Manufacturer + " " + ManufacturerModelName;

    // Store description
    serie->dicom_description = description;

    // Image size
    /*

      int rows = GetTagValueUShort(dset, "Rows");
      int columns = GetTagValueUShort(dset, "Columns");
      // serie->size[0] = columns;
      // serie->size[1] = rows;
      // serie->size[2] = 0; // creation, no file yet
      DD(rows);
      DD(columns);
    */
    DD("TODO size");

    // Image spacing
    /*
      double sz = GetTagValueDouble(dset, "SpacingBetweenSlices");
      if (sz == 0) {
      sz = GetTagValueDouble(dset, "SliceThickness");
      if (sz == 0) sz = 1.0; // not found, default
      }
      std::string spacing = GetStringValueFromTag(dicomIO, "PixelSpacing");
      int n = spacing.find("\\");
      std::string sx = spacing.substr(0,n);
      spacing = spacing.substr(n+1,spacing.size());
      n = spacing.find("\\");
      std::string sy = spacing.substr(0,n);
      // serie->spacing[0] = atof(sx.c_str());
      // serie->spacing[1] = atof(sy.c_str());
      // serie->spacing[2] = sz;
      // if (serie->spacing[0] == 0) serie->spacing[0] = 1.0;
      // if (serie->spacing[1] == 0) serie->spacing[1] = 1.0;
      //  if (sz != 0) serie->spacing[2] = sz; // only update if found
      */
    DD("TODO spacing");

    // other (needed ?)
    // std::string TableTraverse = GetStringValueFromTag(dicomIO, "TableTraverse");
    // std::string InstanceNumber = GetStringValueFromTag(dicomIO, "InstanceNumber");

    // Pixel scale
    double ps = 1.0;
    ps = GetDoubleValueFromTag(dicomIO, "0011|103b"); // PixelScale
    if (ps == 0.0) ps = 1.0;
    serie->dicom_pixel_scale = ps;
    DD(ps);

    // // Duration in sec
    // double d = atof(GetStringValueFromTag(dicomIO, "0018|1242").c_str()); // ActualFrameDuration in msec
    // double f = GetUShortValueFromTag(dicomIO, "0054|0053"); // NumberofFramesinRotation
    // double r = GetUShortValueFromTag(dicomIO, "0054|0051"); // NumberofRotations
    // DD(d);
    // DD(f);
    // DD(r);
    // serie->dicom_duration_sec = d/1000.0*f*r;
    // DD(serie);
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  /// Do not check if the DicomFile and the file already exist
  DicomFile::pointer DicomSerieBuilder::CreateDicomFile(const std::string & filename,
                                                        itk::GDCMImageIO::Pointer dicomIO,
                                                        DicomSerie::pointer serie)
  {
    // First create the file
    syd::File::pointer file;
    db_->New(file);
    std::string f = GetFilenameFromPath(filename);
    file->filename = f;

    std::string relative_folder = serie->ComputeRelativeFolder(); // FIXME
    std::string absolute_folder = db_->ConvertToAbsolutePath(relative_folder);
    if (!fs::exists(absolute_folder)) fs::create_directories(absolute_folder);

    file->path = relative_folder;
    files_to_copy.push_back(filename);
    destination_folders.push_back(absolute_folder);

    // Then create the dicomfile
    syd::DicomFile::pointer dicomfile;
    db_->New(dicomfile);
    dicomfile->file = file;
    dicomfile->dicom_serie = serie;
    std::string sop_uid = GetStringValueFromTag(dicomIO, "0008|0018"); //SOPInstanceUID
    dicomfile->dicom_sop_uid = sop_uid;
    int instance_number = atoi(GetStringValueFromTag(dicomIO, "0020|0013").c_str()); //InstanceNumber
    dicomfile->dicom_instance_number = instance_number;

    // Update the nb of slices
    // int slice = atoi(GetStringValueFromTag(dicomIO, "NumberOfFrames").c_str());
    // if (slice != 0) serie->size[2] = slice;
    // else serie->size[2]++;
    DD("update size");

    return dicomfile;
  }
  // --------------------------------------------------------------------


  // --------------------------------------------------------------------
  void DicomSerieBuilder::InsertDicomSeries()
  {
    // Gather the Files to update
    File::vector files;
    for(auto d:dicomfiles_to_insert) {
      files.push_back(d->file);
    }
    db_->Insert(files);

    // Copy files
    int nb_of_skip_copy=0;
    int n = files_to_copy.size();
    for(auto i=0; i<files_to_copy.size(); i++) {
      std::string f = GetFilenameFromPath(files_to_copy[i]);
      std::string destination = destination_folders[i]+PATH_SEPARATOR+f;
      if (fs::exists(destination)) {
        LOG(4) << "Destination file already exist, ignoring";
        nb_of_skip_copy++;
        continue;
      }
      LOG(3) << "Copying " << f << " to " << destination_folders[i] << std::endl;
      fs::copy_file(files_to_copy[i].c_str(), destination);
      loadbar(i,n);
    }

    // Update the database
    db_->Insert(series_to_insert);
    db_->Insert(dicomfiles_to_insert);

    // Log
    LOG(1) << files.size() << " Files have been added in the db";
    LOG(1) << dicomfiles_to_insert.size() << " DicomFiles have been added in the db";
    LOG(1) << series_to_insert.size() << " DicomSeries has been added in the db";
    if (nb_of_skip_files != 0) {
      LOG(1) << nb_of_skip_files << " dicom already exist in the db and have been skipped.";
    }
    if (nb_of_skip_copy != 0) {
      LOG(1) << nb_of_skip_copy << " files already exist in the db folder and have not been copied.";
    }
    LOG(1) << files_to_copy.size()-nb_of_skip_copy << " files have been copied.";

    // Once done, clear vectors
    series_to_insert.clear();
    dicomfiles_to_insert.clear();
    files.clear();
    files_to_copy.clear();
    destination_folders.clear();
    nb_of_skip_files = 0;
  }
  // --------------------------------------------------------------------


} // end namespace
