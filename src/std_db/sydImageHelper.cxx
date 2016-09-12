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
syd::InsertImageFromFile(std::string filename,
                         syd::Patient::pointer patient,
                         std::string modality)
{
  // New image
  auto db = patient->GetDatabase<syd::StandardDatabase>();
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
  image->files = syd::InsertFilesFromMhd(db, filename,
                                         image->ComputeDefaultRelativePath(),
                                         image->ComputeDefaultMHDFilename());
  // Update size and spacing
  syd::SetImageInfoFromFile(image);
  db->Update(image);
  return image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::File::vector syd::InsertFilesFromMhd(syd::Database * db,
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
  auto image = syd::InsertImageFromFile(temp_filename,
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
  image->AddDicomSerie(dicom);

  // try to guess pixel_unit ?
  auto db = image->GetDatabase<syd::StandardDatabase>();
  syd::PixelUnit::pointer unit = NULL;
  if (dicom->dicom_modality == "CT") {
    try {
      unit = syd::FindPixelUnit(db, "HU");
    } catch(...) {} // ignore if not found
  }
  else {
    try {
      unit = syd::FindPixelUnit(db, "counts");
    } catch(...) {} // ignore if not found
  }

  image->pixel_unit = unit;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SetPixelUnit(syd::Image::pointer image, std::string pixel_unit)
{
  auto db = image->GetDatabase<syd::StandardDatabase>();
  auto u = syd::FindPixelUnit(db, pixel_unit);
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
  auto image = syd::InsertImageFromFile(temp_filename, a->patient, a->dicom_modality);
  DeleteMHDImage(temp_filename);

  // Complete information
  syd::SetImageInfoFromDicomSerie(image, a);
  image->AddDicomSerie(a);
  image->AddDicomSerie(b);
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


// --------------------------------------------------------------------
void syd::SetImageInfoFromImage(syd::Image::pointer image,
                                const syd::Image::pointer like)
{
  image->patient = like->patient;
  image->injection = like->injection;
  image->CopyDicomSeries(like);
  image->type = like->type;
  image->pixel_type = like->pixel_type;
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
syd::Image::pointer
syd::InsertImageGeometricalMean(const syd::Image::pointer input,
                                double k)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  auto itk_input = syd::ReadImage<ImageType>(input->GetAbsolutePath());

  // Check only 4 slices
  int n = itk_input->GetLargestPossibleRegion().GetSize()[2];
  if (n != 4) {
    EXCEPTION("Error I expect 4 slices only: ANT_EM POST_EM ANT_SC POST_SC.");
  }

  std::vector<ImageType::Pointer> itk_images;
  syd::ExtractSlices<ImageType>(itk_input, 2, itk_images); // Direction = Z (2)
  auto ant_em = itk_images[0];
  auto post_em = itk_images[1];
  auto ant_sc = itk_images[2];
  auto post_sc = itk_images[3];
  auto gmean = syd::GeometricalMean<ImageType>(ant_em, post_em, ant_sc, post_sc, k);

  // Create the syd image
  return syd::InsertImage<ImageType>(gmean, input->patient, input->modality);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::CropImageLike(syd::Image::pointer image,
                        const syd::Image::pointer like)
{
  DD("CropImageLike");
  /*
  DD(image);
  DD(like);

  // Read both images
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  auto itk_image = syd::ReadImage<ImageType>(image->GetAbsolutePath());
  auto itk_like = syd::ReadImage<ImageType>(like->GetAbsolutePath());

  // resample like image like image (ouarf!)
  typedef itk::ResampleImageFilter<ImageType, ImageType> FilterType;
  auto t = itk::AffineTransform<double, ImageType::ImageDimension>::New();
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetTransform(t);
  filter->SetSize(like->GetLargestPossibleRegion().GetSize());
  filter->SetOutputSpacing(image->GetSpacing());
  filter->SetOutputOrigin(like->GetOrigin());
  //filter->SetDefaultPixelValue(defaultValue);
  filter->SetOutputDirection(like->GetDirection());
  typename itk::InterpolateImageFunction<ImageType>::Pointer interpolator;
  interpolator = itk::NearestNeighborInterpolateImageFunction<ImageType, double>::New();

  filter->SetInput(input);
  filter->SetInterpolator(interpolator);
  filter->Update();
  auto itk_output = filter->GetOutput();

  // crop

  // save
  */
}

// --------------------------------------------------------------------
