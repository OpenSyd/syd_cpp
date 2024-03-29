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
#include "sydImageProjection.h"
#include "sydImageFillHoles.h"
#include "sydImage_GaussianFilter.h"
#include "sydManualRegistration.h"
#include "sydFAFHelper.h"
#include "sydChangAttenuationImage.h"
#include "sydAttenuationImage.h"
#include "sydTagHelper.h"
#include "sydImageCrop.h"
#include "sydPixelUnitHelper.h"
#include "sydImageRemoveNegativeDirection.h"

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

  // If a direction is negative, resample
  output = syd::ImageRemoveNegativeDirection<ImageType>(output);

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
                                double k, bool crop)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  typedef itk::Image<PixelType, 2> OutputImageType;
  auto itk_input = syd::ReadImage<ImageType>(input->GetAbsolutePath());

  // Check only 4 slices
  int n = itk_input->GetLargestPossibleRegion().GetSize()[2];
  if (n != 4) {
    EXCEPTION("Error I expect 4 slices only: ANT_EM POST_EM ANT_SC POST_SC.");
  }

  std::vector<ImageType::Pointer> itk_images;
  syd::ExtractSlices<ImageType>(itk_input, 2, itk_images); // Direction = Z (2)
  if (crop) {
    auto ant_em = syd::RemoveThirdDimension<ImageType, OutputImageType>(itk_images[0]);
    auto post_em = syd::RemoveThirdDimension<ImageType, OutputImageType>(itk_images[1]);
    auto ant_sc = syd::RemoveThirdDimension<ImageType, OutputImageType>(itk_images[2]);
    auto post_sc = syd::RemoveThirdDimension<ImageType, OutputImageType>(itk_images[3]);
    auto gmean = syd::GeometricalMean<OutputImageType>(ant_em, post_em, ant_sc, post_sc, k);

    // Create the syd image
    return syd::InsertImage<OutputImageType>(gmean, input->patient, input->modality);
  } else {
    auto ant_em = itk_images[0];
    auto post_em = itk_images[1];
    auto ant_sc = itk_images[2];
    auto post_sc = itk_images[3];
    auto gmean = syd::GeometricalMean<ImageType>(ant_em, post_em, ant_sc, post_sc, k);

    // Create the syd image
    return syd::InsertImage<ImageType>(gmean, input->patient, input->modality);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer
syd::InsertDecayCorrectedImage(const syd::Image::pointer input)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  typedef itk::Image<PixelType, 2> OutputImageType;
  auto itk_input = syd::ReadImage<ImageType>(input->GetAbsolutePath());

  syd::Injection::pointer injection = input->injection;
  double time = syd::DateDifferenceInHours(input->acquisition_date, injection->date);
  double lambda = injection->GetLambdaDecayConstantInHours();
	double f = exp(lambda * time); // decay correction: multiply by exp(lambda x time)
	syd::ScaleImage<ImageType>(itk_input, f);
  // Create the syd image
  return syd::InsertImage<ImageType>(itk_input, input->patient, input->modality);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer
syd::InsertProjectionImage(const syd::Image::pointer input,
                           const syd::ImageProjection_Parameters & p)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  typedef itk::Image<PixelType, 2> OutputImageType;
  auto itk_input = syd::ReadImage<ImageType>(input->GetAbsolutePath());
  auto projection = syd::Projection<ImageType, OutputImageType>(itk_input, p);

  // Create the syd image
  return syd::InsertImage<OutputImageType>(projection, input->patient, input->modality);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer
syd::InsertManualRegistration(const syd::Image::pointer inputImage,
                              double x, double y, double z,
                              int center, bool inPlace)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  auto db = inputImage->GetDatabase();
  auto itk_inputImage = syd::ReadImage<ImageType>(inputImage->GetAbsolutePath(), inputImage->dimension);

  //Centerized the image
  if (center != 0) {
    syd::IdType idCenterImage = center;
    syd::Image::pointer centerImage;
    db->QueryOne(centerImage, idCenterImage); // will fail if not found
    LOG(2) << "Read image :" << centerImage;
    auto itk_centerImage = syd::ReadImage<ImageType>(centerImage->GetAbsolutePath(), centerImage->dimension);
    x += itk_centerImage->GetOrigin()[0] + itk_centerImage->GetLargestPossibleRegion().GetSize()[0]*itk_centerImage->GetSpacing()[0]/2
      - itk_inputImage->GetOrigin()[0] - itk_inputImage->GetLargestPossibleRegion().GetSize()[0]*itk_inputImage->GetSpacing()[0]/2 +x;
    y += itk_centerImage->GetOrigin()[1] + itk_centerImage->GetLargestPossibleRegion().GetSize()[1]*itk_centerImage->GetSpacing()[1]/2
      - itk_inputImage->GetOrigin()[1] - itk_inputImage->GetLargestPossibleRegion().GetSize()[1]*itk_inputImage->GetSpacing()[1]/2 +y;
    if (inputImage->dimension >= 3)
      z += itk_centerImage->GetOrigin()[2] + itk_centerImage->GetLargestPossibleRegion().GetSize()[2]*itk_centerImage->GetSpacing()[2]/2
        - itk_inputImage->GetOrigin()[2] - itk_inputImage->GetLargestPossibleRegion().GetSize()[2]*itk_inputImage->GetSpacing()[2]/2 +z;
  }

  //Register the image
  auto imageRegister = syd::ManualRegistration<ImageType>(itk_inputImage, x, y, z);

  // Create the syd image
  if (inPlace) {
    syd::WriteImage<ImageType>(imageRegister, inputImage->GetAbsolutePath(), inputImage->dimension);
    return inputImage;
  } else
    return syd::InsertImage<ImageType>(imageRegister, inputImage->patient, inputImage->modality, inputImage->dimension);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer
syd::InsertFlip(const syd::Image::pointer inputImage,
                std::vector<char> axis, bool flipOrigin)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType3D;
  auto imageFlipped = InsertCopyImage(inputImage);
  auto itk_inputImage = syd::ReadImage<ImageType3D>(imageFlipped->GetAbsolutePath());
  syd::FlipImage<ImageType3D>(itk_inputImage, axis, flipOrigin);
  auto pixel_type = inputImage->pixel_type;
  syd::WriteImage<ImageType3D>(itk_inputImage, imageFlipped->GetAbsolutePath());
  if (pixel_type != "float") {
    // Later --> conversion
    //syd::ConvertImagePixelType(image, args_info.pixel_type_arg);
    LOG(WARNING) << "Pixel type was changed from " << pixel_type
                 << " to float";
  }
  syd::SetImageInfoFromFile(imageFlipped);
  auto db = imageFlipped->GetDatabase();
  db->Update(imageFlipped); // for changed spacing , history
  return(imageFlipped);
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
syd::Image::pointer
syd::InsertChangAttenuation(const syd::Image::pointer inputImage,
                            int nbAngles)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType3D;
  auto itk_inputImage = syd::ReadImage<ImageType3D>(inputImage->GetAbsolutePath());
  auto changImage = syd::ChangAttenuationImage<ImageType3D>(itk_inputImage, nbAngles);

  // Create the syd image
  return syd::InsertImage<ImageType3D>(changImage, inputImage->patient, inputImage->modality);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer
syd::InsertAttenuation(const syd::Image::pointer input, double numberEnergySPECT,
                       double attenuationWaterCT, double attenuationBoneCT,
                       std::vector<double>& attenuationAirSPECT,
                       std::vector<double>& attenuationWaterSPECT,
                       std::vector<double>& attenuationBoneSPECT,
                       std::vector<double>& weight)
{
  // Force to float
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType3D;
  auto itk_inputImage = syd::ReadImage<ImageType3D>(input->GetAbsolutePath());
  auto image = syd::AttenuationImage<ImageType3D>(itk_inputImage, numberEnergySPECT,
                                                  attenuationWaterCT, attenuationBoneCT, attenuationAirSPECT,
                                                  attenuationWaterSPECT, attenuationBoneSPECT, weight);

  // Create the syd image
  return syd::InsertImage<ImageType3D>(image, input->patient, input->modality);
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
void syd::CropImage(syd::Image::pointer image, int * crop)
{
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  auto itk_image = syd::ReadImage<ImageType>(image->GetAbsolutePath());

  // Region
  ImageType::SizeType size;
  size[0] = crop[1]-crop[0];
  size[1] = crop[3]-crop[2];
  size[2] = crop[5]-crop[4];
  ImageType::IndexType start;
  start[0] = crop[0];
  start[1] = crop[2];
  start[2] = crop[4];
  ImageType::RegionType region;
  region.SetSize(size);
  region.SetIndex(start);

  // Crop
  typedef itk::RegionOfInterestImageFilter<ImageType, ImageType> CropFilterType;
  typename CropFilterType::Pointer cropFilter = CropFilterType::New();
  cropFilter->SetInput(itk_image);
  cropFilter->SetRegionOfInterest(region);
  cropFilter->Update();

  // Replace image
  itk_image = cropFilter->GetOutput();
  syd::WriteImage<ImageType>(itk_image, image->GetAbsolutePath());

  // Update image information (size etc)
  syd::SetImageInfoFromFile(image);
  auto db = image->GetDatabase();
  db->Update(image);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ResampleAndCropImageLike(syd::Image::pointer image,
                                   syd::Image::pointer like,
                                   int interpolationType,
                                   double defaultValue)
{
  // Read input image
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  auto itk_image = syd::ReadImage<ImageType>(image->GetAbsolutePath());
  auto itk_like = syd::ReadImage<ImageType>(like->GetAbsolutePath());

  // Create fake 'like' image
  itk_image = syd::ResampleAndCropImageLike<ImageType>(itk_image, itk_like, interpolationType, defaultValue);

  // Replace image
  syd::WriteImage<ImageType>(itk_image, image->GetAbsolutePath());

  // Update image information (size etc)
  syd::SetImageInfoFromFile(image);
  auto db = image->GetDatabase();
  db->Update(image);
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
                                 syd::Injection::pointer injection)
{
  if (image->injection == NULL) {
    EXCEPTION("Cannot SubstituteRadionuclide because the image is not associated with an injection: "
              << image);
  }

  // Set new injection
  auto db = image->GetDatabase<syd::StandardDatabase>();

  // Get the time and the half_life (lambda)
  double time = syd::DateDifferenceInHours(image->acquisition_date, image->injection->date);
  double lambda_old = image->injection->radionuclide->GetLambdaDecayConstantInHours();
  double lambda_new = injection->radionuclide->GetLambdaDecayConstantInHours();
  double f1 = exp(lambda_old * time); // decay correction: multiply by exp(lambda x time)
  double f2 = exp(-lambda_new * time); // new radionuclide decay

  // substitute the radionuclide taking into account the half life
  syd::ScaleImage(image, f1*f2);
  image->injection = injection;
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
syd::Image::vector syd::FindImages(const syd::Injection::pointer injection)
{
  auto db = injection->GetDatabase();
  odb::query<syd::Image> q = odb::query<syd::Image>::injection == injection->id;
  syd::Image::vector images;
  db->Query(images, q);
  return images;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::vector syd::FindImages(syd::StandardDatabase * db, const std::vector<int> ids)
{
  syd::Image::vector images;
  if (ids.size() != 0) {
    odb::query<syd::Image> q = odb::query<syd::Image>::id == ids[0];
    for (int i = 1; i<ids.size(); ++i)
      q = q || odb::query<syd::Image>::id == ids[i];
    db->Query(images, q);
  }
  return images;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Move(syd::Image::pointer image, std::string relative_folder)
{
  // Get old path
  auto old_file = image->GetAbsolutePath();
  if (image->files.size() == 0) return;
  auto old_path = image->files[0]->path;

  // Set new one
  for(auto f:image->files) f->path = relative_folder;

  // Get new path
  auto new_file = image->GetAbsolutePath();
  if (new_file == old_file) return; // do noting

  if (fs::exists(new_file)) {
    LOG(WARNING) << "File exist, cannot overwrite " << new_file;
    for(auto f:image->files) f->path = old_path;
    return;
  }

  // Create folder (if does not exist)
  auto db = image->GetDatabase();
  auto absolute_folder = db->ConvertToAbsolutePath(relative_folder);
  fs::create_directories(absolute_folder);

  // Rename
  if (image->type == "mhd") {
    syd::RenameMHDImage(old_file, new_file, false);
  }
  else {
    fs::rename(old_file, new_file);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
odb::query<syd::Image> syd::QueryImage(syd::Patient::pointer patient)
{
  odb::query<syd::Image> q = odb::query<syd::Image>::patient == patient->id;
  return q;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
odb::query<syd::Image> syd::QueryImageModality(std::string modalities)
{
  std::vector<std::string> mod;
  syd::GetWords(mod, modalities);
  odb::query<syd::Image> q = odb::query<syd::Image>::modality.in_range(mod.begin(), mod.end());
  return q;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
odb::query<syd::Image> syd::QueryImagePixelUnit(std::string pixel_unit)
{
  odb::query<syd::Image> q = odb::query<syd::Image>::pixel_unit->name == pixel_unit;
  return q;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::vector syd::FindImages(syd::Patient::pointer patient,
                                   odb::query<syd::Image> q,
                                   const syd::Tag::vector & tags)
{
  auto db = patient->GetDatabase<syd::StandardDatabase>();
  syd::Image::vector images;
  q = q and QueryImage(patient);
  db->Query(images, q);
  images = syd::GetRecordsThatContainAllTags<syd::Image>(images, tags);
  return images;
}
// --------------------------------------------------------------------


// --------------------------------------------------
bool AlmostEquals(double a, double b, double tol = 0.000001)
{
  //std::numeric_limits<double>::epsilon();
  return std::abs(a - b) < tol;
}
// --------------------------------------------------


// --------------------------------------------------------------------
bool syd::SameSizeAndSpacing(const syd::Image::pointer a,
                             const syd::Image::pointer b)
{
  bool r = true;
  for(auto i=0; i<a->size.size(); i++) {
    r = r and
      AlmostEquals(a->size[i], b->size[i]) and
      AlmostEquals(a->spacing[i], b->spacing[i]);
    if (!r) return r;
  }
  return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::SameSizeAndSpacing(const syd::Image::pointer image,
                             const syd::DicomSerie::pointer serie)
{
  bool b = true;
  for(auto i=0; i<image->size.size(); i++) {
    b = b and
      AlmostEquals(image->size[i], serie->dicom_size[i]) and
      AlmostEquals(image->spacing[i], serie->dicom_spacing[i]);
    if (!b) return b;
  }
  return b;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::vector syd::FindImagesLike(syd::DicomSerie::pointer serie)
{
  auto db = serie->GetDatabase<syd::StandardDatabase>();
  syd::Image::vector temp;
  odb::query<syd::Image> q =
    // odb::query<syd::Image>::patient == serie->patient->id and
    //odb::query<syd::Image>::injection == serie->injection->id and
    odb::query<syd::Image>::frame_of_reference_uid == serie->dicom_frame_of_reference_uid and
    odb::query<syd::Image>::acquisition_date == serie->dicom_acquisition_date and
    odb::query<syd::Image>::modality == serie->dicom_modality;
  db->Query(temp, q);
  syd::Image::vector images;
  // Check dicom id exist in the list
  for(auto image:temp) {
    auto it = std::find(image->dicoms.begin(), image->dicoms.end(), serie);
    if (it == image->dicoms.end()) continue; // does not exist: we continue the loop
    if (syd::SameSizeAndSpacing(image, serie)) images.push_back(image); // kepp only if found
  }
  return images;
}
// --------------------------------------------------------------------
