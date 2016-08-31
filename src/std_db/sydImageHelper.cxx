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
#include "sydImageHelper.h"
#include "sydFileHelper.h"

// --------------------------------------------------------------------
syd::Image::pointer
syd::InsertMhdImage(std::string filename,
                    syd::Patient::pointer patient,
                    std::string modality)
{
  auto db = patient->GetDatabase<syd::StandardDatabase>();

  // New image
  syd::Image::pointer image;
  db->New(image);

  // default information
  image->patient = patient;
  image->modality = modality;
  image->type = "mhd";

  // insert (without mhd yet) to get an id
  db->Insert(image);

  // copy mhd to db according to default filename and create the two
  // associated syd::File
  image->files = syd::InsertMhdFiles(db, filename,
                                     GetDefaultImageRelativePath(image),
                                     GetDefaultMhdImageFilename(image));
  // Update size and spacing
  DD("todo udpate size and spacing");
  syd::SetImageInfoFromFile(image);

  // Check for negative spacing and orientation
  DD("syd::FlipImageIfNegativeDirection(image);");

  DD("Set Image md5 ???");

  db->Update(image);
  DD(image);
  return image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::GetDefaultImageRelativePath(syd::Image::pointer image)
{
  if (image->patient == NULL) {
    EXCEPTION("Cannot compute the default image relative path"
              << ", no patient ar yet associated with the image: "
              << image);
  }
  auto s = image->patient->name;
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
std::string syd::GetDefaultMhdImageFilename(syd::Image::pointer image)
{
  if (!image->IsPersistent()) {
    EXCEPTION("Image must be persistent (in the db) to "
              << "use GetDefaultMhdImageFilename.");
  }
  std::ostringstream oss;
  oss << image->modality << "_" << image->id << ".mhd";
  return oss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::File::vector syd::InsertMhdFiles(syd::Database * db,
                                      std::string from_filename,
                                      std::string to_relative_path,
                                      std::string to_filename)
{
  // Get the 'raw' filename
  std::string raw_filename = to_filename;
  syd::Replace(raw_filename, ".mhd", ".raw");

  // Create files
  syd::File::pointer mhd_file, raw_file;
  db->New(mhd_file);
  db->New(raw_file);
  mhd_file->path = to_relative_path;
  raw_file->path = to_relative_path;
  mhd_file->filename = to_filename;
  raw_file->filename = raw_filename;
  DD(mhd_file);
  DD(raw_file);

  // Copy to the db
  DD(mhd_file->GetAbsolutePath());
  syd::CopyMHDImage(from_filename,
                    mhd_file->GetAbsolutePath(),
                    db->GetOverwriteFileFlag());

  // Insert into the db
  syd::File::vector files;
  files.push_back(mhd_file);
  files.push_back(raw_file);
  db->Insert(files);

  DD("done");
  return files;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SetImageInfoFromFile(syd::Image::pointer image)
{
  DD("UpdateImageInfoFromFile");
  auto header = syd::ReadImageHeader(image->GetAbsolutePath());
  auto pixel_type = itk::ImageIOBase::GetComponentTypeAsString(header->GetComponentType());
  DD(pixel_type);
  image->pixel_type = pixel_type;
  image->dimension = header->GetNumberOfDimensions();
  image->spacing.clear();
  image->size.clear();
  for(int i=0; i<image->dimension; i++) {
    image->spacing.push_back(header->GetSpacing(i));
    image->size.push_back(header->GetDimensions(i));
  }
  DD(image);
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
syd::Image::pointer syd::InsertImageFromDicom(syd::DicomSerie::pointer dicom,
                                              std::string user_pixel_type)
{
  DD(dicom);

  // Get dicom associated files or folder
  auto dicom_files = dicom->dicom_files;
  if (dicom_files.size() == 0) {
    EXCEPTION("Error no DicomFile associated with this DicomSerie: " << dicom);
  }
  std::vector<std::string> dicom_filenames;
  for(auto f:dicom_files)
    dicom_filenames.push_back(f->GetAbsolutePath());

  // Consider pixel type (from user or from the file)
  std::string pixel_type = user_pixel_type;
  if (pixel_type == "auto") {
    // read dicom header with itk header (first file only)
    auto header = syd::ReadImageHeader(dicom_files[0]->GetAbsolutePath());
    pixel_type =
      itk::ImageIOBase::GetComponentTypeAsString(header->GetComponentType());
  }
  DD(pixel_type);

  // Create a temporary filename
  auto db = dicom->GetDatabase<syd::StandardDatabase>();
  std::string t = db->GetDatabaseAbsoluteFolder()+PATH_SEPARATOR+"syd_temp_%%%%_%%%%_%%%%_%%%%.mhd";
  fs::path p = fs::unique_path(t);
  std::string temp_filename = p.string();
  DD(temp_filename);

  //
  if (pixel_type == "float") syd::WriteDicomToMhd<float>(dicom, temp_filename);
  if (pixel_type == "short") syd::WriteDicomToMhd<short>(dicom, temp_filename);
  DD("TO change WriteDicomToMhd without template");

  // Attach the mhd to the Image
  auto image = syd::InsertMhdImage(temp_filename,
                                   dicom->patient,
                                   dicom->dicom_modality);
  syd::DeleteMHDImage(temp_filename);

  // Set the image properties
  image->modality = dicom->dicom_modality; // needed to set the filename
  image->acquisition_date = dicom->dicom_acquisition_date;
  image->frame_of_reference_uid = dicom->dicom_frame_of_reference_uid;
  image->dicoms.push_back(dicom);
  // pixel unit
  DD("pixe unit todo");
  DD(image);

  // Last update
  db->Update(image);
  return image;
}
// --------------------------------------------------------------------









// OLD BELOW



// --------------------------------------------------------------------
/*void syd::ImageHelper::
  InsertMhdFiles(syd::Image::pointer image, std::string filename, bool moveFlag)
  {
  if (!image->IsPersistent()) {
  EXCEPTION("Image not in the db. Can only InsertMhdFiles for persistent image. Usedb->Insert(image) first.");
  }
  auto db = image->GetDatabase();
  // Need to clear the associated files and delete them (after update)
  syd::File::vector previous_files = image->files;
  image->files.clear();
  fs::path p(filename);
  if (p.extension() != ".mhd") {
  EXCEPTION("Extension must be .mhd, cannot InsertMhdFiles.");
  }
  image->type = "mhd";
  auto path = image->ComputeRelativeFolder();
  auto mhd_filename = image->ComputeDefaultMhdFilename();
  syd::File::pointer file_mhd = syd::FileHelper::New(db, path, mhd_filename);
  std::string raw = mhd_filename;
  syd::Replace(raw, ".mhd", ".raw");
  syd::File::pointer file_raw = syd::FileHelper::New(db, path, raw);
  image->files.push_back(file_mhd);
  image->files.push_back(file_raw);
  db->Insert(file_mhd);
  db->Insert(file_raw);
  if (!moveFlag)
  CopyMHDImage(filename, image->GetAbsolutePath()); // copy files in the db
  else
  RenameMHDImage(filename, image->GetAbsolutePath());
  syd::ImageHelper::UpdateMhdImageProperties(image);
  db->Update(image);
  db->Delete(previous_files);
  }*/
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageHelper::
CopyInformation(syd::Image::pointer image, const syd::Image::pointer like)
{
  image->patient = like->patient;
  image->injection = like->injection;
  image->CopyDicomSeries(like);
  // image->type = like->type; // not copied (depends on the file)
  // image->pixel_type = like->pixel_type; // not copied (depends on the file)
  image->pixel_unit = like->pixel_unit;
  image->frame_of_reference_uid = like->frame_of_reference_uid;
  image->acquisition_date = like->acquisition_date;
  image->modality = like->modality;
  image->tags.clear();
  for(auto t:like->tags) image->tags.push_back(t);
  // (The history is not copied)
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageHelper::
SetPixelUnit(syd::Image::pointer image, std::string pixel_unit)
{
  auto db = image->GetDatabase<syd::StandardDatabase>();
  auto u = db->FindPixelUnit(pixel_unit);
  image->pixel_unit = u;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageHelper::
SetInjection(syd::Image::pointer image, std::string injection)
{
  auto db = image->GetDatabase<syd::StandardDatabase>();
  auto i = db->FindInjection(image->patient, injection);
  image->injection = i;
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
void syd::ImageHelper::
AddDicomSerie(syd::Image::pointer image, syd::IdType id)
{
  auto db = image->GetDatabase<syd::StandardDatabase>();
  syd::DicomSerie::pointer d;
  db->QueryOne(d, id);
  image->AddDicomSerie(d);
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
void syd::ImageHelper::
UpdateMhdImageProperties(syd::Image::pointer image)
{
  auto filename = image->GetAbsolutePath();
  auto header = syd::ReadImageHeader(filename);
  if (!header) {
    EXCEPTION("Internal error cannot read the file " << filename);
  }
  UpdateMhdImageProperties(image, header);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageHelper::
UpdateMhdImageProperties(syd::Image::pointer image, itk::ImageIOBase::Pointer header)
{
  // Check PixelType = scalar
  image->pixel_type = itk::ImageIOBase::GetComponentTypeAsString(header->GetComponentType());
  auto d = image->dimension = header->GetNumberOfDimensions();
  image->size.clear();
  image->spacing.clear();
  for(auto i=0; i<d; i++) {
    image->size.push_back(header->GetDimensions(i));
    image->spacing.push_back(header->GetSpacing(i));
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::ImageHelper::IsSameImage(syd::Image::pointer a,
                                   syd::Image::pointer b,
                                   bool checkHistoryFlag)
{
  if (checkHistoryFlag) {
    a->SetPrintHistoryFlag(true);
    b->SetPrintHistoryFlag(true);
  }
  else {
    a->SetPrintHistoryFlag(false);
    b->SetPrintHistoryFlag(false);
  }
  return (a == b);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageHelper::CheckSameImageAndFiles(syd::Image::pointer a,
                                              syd::Image::pointer b)
{
  if  (!syd::ImageHelper::IsSameImage(a,b)) {
    EXCEPTION("Error images are different: " << std::endl
              << a << std::endl << b);
  }

  for(auto i=0; i<a->files.size(); i++) {
    std::string fa = a->files[i]->GetAbsolutePath();
    std::string fb = b->files[i]->GetAbsolutePath();
    if (!syd::EqualFiles(fa, fb)) {
      EXCEPTION("Error images files are different: "
                << fa << " " << fb << std::endl
                << "For images: " << std::endl
                << a << std::endl << b);
    }
  }
}
// --------------------------------------------------------------------
