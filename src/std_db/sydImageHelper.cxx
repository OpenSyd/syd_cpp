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
#include "sydRoiMaskImageHelper.h"
#include "sydInjectionHelper.h"
#include "sydImageStitch.h"
#include "sydRoiStatisticHelper.h"
#include "sydImageFillHoles.h"
#include "sydImage_GaussianFilter.h"
#include "sydManualRegistration.h"
#include "sydFlip.h"

// --------------------------------------------------------------------
syd::Image::pointer
syd::InsertImageFromFile(std::string filename,
                         syd::Patient::pointer patient,
                         std::string modality)
{
  // New image
  auto db = patient->GetDatabase();
  auto image = db->New<syd::Image>();

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
  auto mhd_file = db->New<syd::File>();
  auto raw_file = db->New<syd::File>();
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
  auto db = dicom->GetDatabase();
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
  image->injection = dicom->injection;

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
  auto db = image->GetDatabase();
  auto i = syd::FindInjection(image->patient, injection);
  image->injection = i;
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
void syd::AddDicomSerie(syd::Image::pointer image, syd::IdType id)
{
  auto db = image->GetDatabase();
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
  auto db = image->GetDatabase();
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
  auto db = a->GetDatabase();
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
  image->comments.clear();
  for(auto c:like->comments) image->comments.push_back(c);
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
syd::Image::pointer
syd::InsertManualRegistration(const syd::Image::pointer inputImage,
                              double x, double y, double z)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType3D;
  auto itk_inputImage = syd::ReadImage<ImageType3D>(inputImage->GetAbsolutePath());
  auto imageRegister = syd::ManualRegistration<ImageType3D>(itk_inputImage, x, y, z);

  // Create the syd image
  return syd::InsertImage<ImageType3D>(imageRegister, inputImage->patient, inputImage->modality);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer
syd::InsertFlip(const syd::Image::pointer inputImage,
                 int axis, bool flipOrigin)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType3D;
  auto itk_inputImage = syd::ReadImage<ImageType3D>(inputImage->GetAbsolutePath());
  auto imageFlipped = syd::Flip<ImageType3D>(itk_inputImage, axis, flipOrigin);

  // Create the syd image
  return syd::InsertImage<ImageType3D>(imageFlipped, inputImage->patient, inputImage->modality);
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


// --------------------------------------------------------------------
double syd::ComputeActivityInMBqByDetectedCounts(syd::Image::pointer image)
{
  if (image->injection == NULL) {
    EXCEPTION("Cannot ComputeActivityInMBqByDetectedCounts, need an injection for this image");
  }

  //   FIXME check pixel type = counts (warn or forcae
  // inverse ComputeDetectedCountsByAcitvityInMBq

  auto injection = image->injection;
  double injected_activity = injection->activity_in_MBq;
  double time = syd::DateDifferenceInHours(image->acquisition_date, injection->date);
  double lambda = log(2.0)/(injection->radionuclide->half_life_in_hours);
  double activity_at_acquisition = injected_activity * exp(-lambda * time);

  // Compute stat (without mask)
  auto db = image->GetDatabase();
  auto stat = db->New<syd::RoiStatistic>();
  stat->image = image;
  syd::ComputeRoiStatistic(stat);

  // activity by nb of counts
  double s = activity_at_acquisition / stat->sum;
  return s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::FlipImageIfNegativeSpacing(syd::Image::pointer image)
{
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  auto itk_image = syd::ReadImage<ImageType>(image->GetAbsolutePath());
  bool flip = syd::FlipImageIfNegativeSpacing<ImageType>(itk_image);
  if (flip) {
    auto pixel_type = image->pixel_type;
    syd::WriteImage<ImageType>(itk_image, image->GetAbsolutePath());
    if (pixel_type != "float") {
      // Later --> conversion
      //syd::ConvertImagePixelType(image, args_info.pixel_type_arg);
      LOG(WARNING) << "Pixel type was changed from " << pixel_type
                   << " to float";
    }
    syd::SetImageInfoFromFile(image);
    auto db = image->GetDatabase();
    db->Update(image); // for changed spacing , history
  }
  return flip;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::InsertCopyImage(syd::Image::pointer image)
{
  auto output = syd::InsertImageFromFile(image->GetAbsolutePath(),
                                         image->patient,
                                         image->modality);
  syd::SetImageInfoFromImage(output, image);
  auto db = image->GetDatabase();
  db->Update(output);
  return output;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::SubstituteRadionuclide(syd::Image::pointer image,
                                 syd::Radionuclide::pointer rad)
{
  if (image->injection == NULL) {
    EXCEPTION("Cannot SubstituteRadionuclide because the image is not associated with an injection: "
              << image);
  }

  // Create new injection
  auto db = image->GetDatabase<syd::StandardDatabase>();
  auto new_injection = syd::CopyInjection(image->injection);
  new_injection->radionuclide = rad;
  auto inj = syd::GetSimilarInjection(db, new_injection);
  if (inj.size() != 0) {
    LOG(2) << "Similar injection exist, do not add " << std::endl
           << new_injection << std::endl
           << inj[0];
    new_injection = inj[0];
  }
  else db->Insert(new_injection);

  // Get the time and the half_life (lambda)
  double time = syd::DateDifferenceInHours(image->acquisition_date, image->injection->date);
  double lambda_old = image->injection->radionuclide->GetLambdaDecayConstantInHours();
  double lambda_new = rad->GetLambdaDecayConstantInHours();
  double f1 = exp(lambda_old * time); // decay correction: multiply by exp(lambda x time)
  double f2 = exp(-lambda_new * time); // new radionuclide decay

  // substitute the radionuclide taking into account the half life
  syd::ScaleImage(image, f1*f2);
  image->injection = new_injection;
  db->Update(image);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::vector<double> syd::GetTimesFromInjection(const syd::Image::vector images)
{
  std::vector<double> times;
  for(auto image:images) {
    if (image->injection == nullptr) {
      EXCEPTION("Error no injection for image : " << image);
    }
    auto starting_date = image->injection->date;
    double t = syd::DateDifferenceInHours(image->acquisition_date, starting_date);
    times.push_back(t);
  }
  return times;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::FillHoles(syd::Image::pointer image,
                    syd::Image::pointer mask,
                    int radius,
                    double mask_value,
                    int & nb_failures,
                    int & nb_changed)
{
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  auto input_itk = syd::ReadImage<ImageType>(image->GetAbsolutePath());
  auto mask_itk = syd::ReadImage<ImageType>(mask->GetAbsolutePath());
  syd::FillHoles<ImageType>(input_itk, mask_itk, radius, mask_value, nb_failures, nb_changed);
  syd::WriteImage<ImageType>(input_itk, image->GetAbsolutePath());
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ApplyGaussianFilter(syd::Image::pointer image, double sigma_in_mm)
{
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  auto itk_image = syd::ReadImage<ImageType>(image->GetAbsolutePath());
  itk_image = syd::GaussianFilter<ImageType>(itk_image, sigma_in_mm);
  syd::WriteImage<ImageType>(itk_image, image->GetAbsolutePath());

  // update the image to set the history
  auto db = image->GetDatabase();
  db->Update(image);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::vector syd::FindImages(syd::StandardDatabase * db, const std::string & patient_name)
{
  return syd::FindImages(db->FindPatient(patient_name));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::vector syd::FindImages(const syd::Patient::pointer patient)
{
  auto db = patient->GetDatabase();
  odb::query<syd::Image> q = odb::query<syd::Image>::patient == patient->id;
  syd::Image::vector images;
  db->Query(images, q);
  return images;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::vector syd::FindImagesFromDicom(const syd::DicomSerie::pointer dicom)
{
  auto patient = dicom->patient;
  auto images = syd::FindImages(patient);
  syd::Image::vector images_from_dicom;
  for(auto im:images) {
    bool imageIsFromThisDicom = false;
    for(auto d:im->dicoms) if (d->id == dicom->id) imageIsFromThisDicom = true;
    if (imageIsFromThisDicom) images_from_dicom.push_back(im);
  }
  return images_from_dicom;
}
// --------------------------------------------------------------------
