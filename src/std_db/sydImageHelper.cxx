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
#include "sydPixelUnitHelper.h"

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
  syd::SetImageInfoFromFile(image);

  // Check for negative spacing and orientation
  DD("syd::FlipImageIfNegativeDirection(image);");

  DD("Set Image md5 ???");

  db->Update(image);
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

  // Copy to the db
  syd::CopyMHDImage(from_filename,
                    mhd_file->GetAbsolutePath(),
                    db->GetOverwriteFileFlag());

  // Insert into the db
  syd::File::vector files;
  files.push_back(mhd_file);
  files.push_back(raw_file);
  db->Insert(files);

  return files;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SetImageInfoFromFile(syd::Image::pointer image)
{
  auto header = syd::ReadImageHeader(image->GetAbsolutePath());
  auto pixel_type = itk::ImageIOBase::GetComponentTypeAsString(header->GetComponentType());
  image->pixel_type = pixel_type;
  image->dimension = header->GetNumberOfDimensions();
  image->spacing.clear();
  image->size.clear();
  for(int i=0; i<image->dimension; i++) {
    image->spacing.push_back(header->GetSpacing(i));
    image->size.push_back(header->GetDimensions(i));
  }
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
syd::Image::pointer syd::InsertImageFromDicomSerie(syd::DicomSerie::pointer dicom,
                                                   std::string pixel_type)
{
  // Get dicom associated files or folder
  auto dicom_files = dicom->dicom_files;
  if (dicom_files.size() == 0) {
    EXCEPTION("Error no DicomFile associated with this DicomSerie: " << dicom);
  }
  std::vector<std::string> dicom_filenames;
  for(auto f:dicom_files)
    dicom_filenames.push_back(f->GetAbsolutePath());

  // Create a temporary filename
  auto db = dicom->GetDatabase<syd::StandardDatabase>();
  std::string temp_filename = db->GetUniqueTempFilename();

  // Convert the dicom to a mhd
  syd::WriteDicomToMhd(dicom, pixel_type, temp_filename);

  // Attach the mhd to the Image
  auto image = syd::InsertMhdImage(temp_filename,
                                   dicom->patient,
                                   dicom->dicom_modality);
  syd::DeleteMHDImage(temp_filename);

  // Set some image properties
  syd::SetImageInfoFromDicomSerie(image, dicom);

  // Last update
  db->Update(image);
  return image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SetImageInfoFromDicomSerie(syd::Image::pointer image,
                                     const syd::DicomSerie::pointer dicom)
{
  image->patient = dicom->patient;
  image->modality = dicom->dicom_modality;
  image->acquisition_date = dicom->dicom_acquisition_date;
  image->frame_of_reference_uid = dicom->dicom_frame_of_reference_uid;

  // try to guess pixel_unit ?
  auto db = image->GetDatabase<syd::StandardDatabase>();
  syd::PixelValueUnit::pointer unit = NULL;
  if (dicom->dicom_modality == "CT") {
    try {
      unit = syd::PixelUnitHelper::FindPixelUnit(db, "HU");
    } catch(...) {} // ignore if not found
  }
  else {
    try {
      unit = syd::PixelUnitHelper::FindPixelUnit(db, "counts");
    } catch(...) {} // ignore if not found
  }

  image->pixel_unit = unit;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SetPixelUnit(syd::Image::pointer image, std::string pixel_unit)
{
  auto db = image->GetDatabase<syd::StandardDatabase>();
  auto u = db->FindPixelUnit(pixel_unit);
  image->pixel_unit = u;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SetInjection(syd::Image::pointer image, std::string injection)
{
  auto db = image->GetDatabase<syd::StandardDatabase>();
  auto i = db->FindInjection(image->patient, injection);
  image->injection = i;
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
void syd::AddDicomSerie(syd::Image::pointer image, syd::IdType id)
{
  auto db = image->GetDatabase<syd::StandardDatabase>();
  syd::DicomSerie::pointer d;
  db->QueryOne(d, id);
  image->AddDicomSerie(d);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ScaleImage(syd::Image::pointer image, double s)
{
  // force float image
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  auto itk_image = syd::ReadImage<ImageType>(image->GetAbsolutePath());
  syd::ScaleImage<ImageType>(itk_image, s);
  syd::WriteImage<ImageType>(itk_image, image->GetAbsolutePath());
  image->pixel_type = "float";
  auto db = image->GetDatabase<syd::StandardDatabase>();
  db->Update(image); // to change the history (on the pixel_type)
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::CheckSameImageAndFiles(syd::Image::pointer a,
                                 syd::Image::pointer b)
{
  if  (!syd::IsSameImage(a,b)) {
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
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::InsertStitchDicomImage(syd::DicomSerie::pointer a,
                                                syd::DicomSerie::pointer b,
                                                double threshold_cumul,
                                                double skip_slices)
{
  // Read the dicom images (force to float)
  auto db = a->GetDatabase<syd::StandardDatabase>();
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  auto image_a = syd::ReadDicomSerieImage<ImageType>(a);
  auto image_b = syd::ReadDicomSerieImage<ImageType>(b);

  // Stitch (default values for now, to be changed!)
  auto output = syd::StitchImages<ImageType>(image_a, image_b,
                                             threshold_cumul, skip_slices);

  // Write on disk
  std::string temp_filename = db->GetUniqueTempFilename();
  syd::WriteImage<ImageType>(output, temp_filename);

  // Create image
  auto image = syd::InsertMhdImage(temp_filename, a->patient, a->dicom_modality);
  DeleteMHDImage(temp_filename);

  // Complete information
  syd::SetImageInfoFromDicomSerie(image, a);
  image->dicoms.push_back(a);
  image->dicoms.push_back(b);
  db->Update(image);
  return image;
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
bool syd::IsSameImage(syd::Image::pointer a,
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

/*
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


*/
