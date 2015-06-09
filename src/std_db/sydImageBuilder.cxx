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
#include "sydImageBuilder.h"
#include "sydTable.h"
#include "sydImageUtils.h"

// --------------------------------------------------------------------
syd::ImageBuilder::ImageBuilder(syd::StandardDatabase * db):syd::ImageBuilder()
{
  SetDatabase(db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::ImageBuilder::ImageBuilder()
{
  db_ = NULL;
  //tag_.label = "unamed_tag";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image syd::ImageBuilder::InsertImage(const syd::Tag & tag, const syd::DicomSerie & dicomserie)
{
  // Get the files
  std::vector<syd::DicomFile> dicom_files;
  db_->Query<syd::DicomFile>(odb::query<syd::DicomFile>::dicom_serie->id == dicomserie.id, dicom_files);
  if (dicom_files.size() == 0) {
    EXCEPTION("Error not DicomFile associated with this DicomSerie: " << dicomserie);
  }
  std::vector<std::string> dicom_filenames;
  for(auto f:dicom_files) {
    dicom_filenames.push_back(db_->GetAbsolutePath(*f.file));
  }

  // Create image first to get the id
  syd::Image image = InsertEmptyImage(tag, *dicomserie.patient);
  UpdateDicom(image, dicomserie);
  UpdateFile(image, GetDefaultImageRelativePath(image), GetDefaultImageFilename(image));
  std::string f = db_->GetAbsolutePath(image);

  // Convert dicom, write to disk
  // std::string md5;
  try {
    if (dicomserie.dicom_modality == "CT") {
      typedef short PixelType;
      typedef itk::Image<PixelType,3> ImageType;
      ImageType::Pointer itk_image = ReadImage<PixelType>(dicomserie);
      UpdateImageInfo<PixelType>(image, itk_image, true); // true = update md5
      syd::WriteImage<ImageType>(itk_image, f);
    }
    else {
      typedef float PixelType;
      typedef itk::Image<PixelType,3> ImageType;
      ImageType::Pointer itk_image = ReadImage<PixelType>(dicomserie);
      UpdateImageInfo<PixelType>(image, itk_image, true); // true = update md5
      syd::WriteImage<ImageType>(itk_image, f);
    }

    // Insert in the db
    db_->Update(image);

  } catch (syd::Exception & e) {
    db_->Delete(image);
    EXCEPTION("Error during InsertImage: " << e.what() << " (I remove the image " << image << ")");
  }

  return image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image syd::ImageBuilder::InsertStitchedImage(const syd::Tag & tag,
                                                  const syd::DicomSerie & a,
                                                  const syd::DicomSerie & b)
{
  // Check a and b modality
  if (a.dicom_modality != b.dicom_modality) {
    LOG(FATAL) << "Error while trying to stitch the two following images, dicom_modality is not the same: "
               << std::endl << a
               << std::endl << b;
  }

  // Only consider single file and not CT for the moment
  if (a.dicom_modality == "CT") {
    LOG(FATAL) << "Error cannot stitch CT images. Only float pixel type (yet).";
  }

  // Check if the same serie (flag to by pass
  if (a.dicom_series_uid != b.dicom_series_uid) {
    LOG(FATAL) << "Error, cannot stitch those two dicoms because the series_uid is not the same:"
               << std::endl << a << " " << a.dicom_series_uid
               << std::endl << b << " " << b.dicom_series_uid;
  }

  // Create the image record
  syd::Image image = InsertEmptyImage(tag, *a.patient);
  UpdateDicom(image, a);
  UpdateDicom(image, b);
  UpdateFile(image, GetDefaultImageRelativePath(image), GetDefaultImageFilename(image));

  // Read the dicom images
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  ImageType::Pointer image_a = ReadImage<PixelType>(a);
  ImageType::Pointer image_b = ReadImage<PixelType>(b);

  // Stitch (default values for now, to be changed!)
  ImageType::Pointer output = syd::StitchImages<ImageType>(image_a, image_b, 150000, 4);

  // Update the image values
  UpdateImageInfo<PixelType>(image, output, true); // true = update md5

  // Write image
  syd::WriteImage<ImageType>(output, db_->GetAbsolutePath(image));

  // Insert into the db
  db_->Update(image);
  return image;
}
// --------------------------------------------------------------------




// --------------------------------------------------------------------
void syd::ImageBuilder::InsertImagesFromTimepoint(syd::Timepoint & timepoint)
{

  DD("InsertImagesFromTimepoint TODO");
  exit(0);

  // // loop create all images
  // for(auto d:timepoint.dicoms) {
  //   // Create new image
  //   syd::Image image = InsertImage(*d);
  //   // Associate image with timepoint
  //   timepoint.images.push_back(std::make_shared<syd::Image>(image));
  // }
  // db_->Update(timepoint);
  // //  LOG(1) << "Timpoint (" << timepoint << ") updated with " << timepoint.dicoms.size() << " images."; // FIXME not here
}
// --------------------------------------------------------------------

/* from file ?
   itk::ImageIOBase::Pointer imageIO =
   itk::ImageIOFactory::CreateImageIO(
   inputFileName,
   itk::ImageIOFactory::ReadMode );
   imageIO->SetFileName( inputFileName );
   imageIO->ReadImageInformation();
   http://itk.org/ITKExamples/src/IO/ImageBase/ReadUnknownImageType/Documentation.html
*/


// --------------------------------------------------------------------
std::string syd::ImageBuilder::GetDefaultImageFilename(const syd::Image & image)
{
  if (image.dicoms.size() < 1) {
    LOG(FATAL) << "Error cannot use GetDefaultImageFilename on image without dicom. Image: " << image;
  }
  std::ostringstream oss;
  oss << image.dicoms[0]->dicom_modality << "_" << image.id << ".mhd";
  std::string filename = oss.str();
  return filename;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ImageBuilder::GetDefaultRoiMaskImageFilename(const syd::RoiMaskImage & mask)
{
  if (mask.roitype == NULL) {
    LOG(FATAL) << "Error cannot use GetDefaultRoiMaskImageFilename without roitype. Mask: " << mask;
  }
  std::ostringstream oss;
  oss << mask.roitype->name << "_" << mask.id << ".mhd";
  std::string filename = oss.str();
  return filename;
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
std::string syd::ImageBuilder::GetDefaultImageRelativePath(const syd::Image & image)
{
  return db_->GetRelativeFolder(*image.patient);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ImageBuilder::GetDefaultRoiMaskImageRelativePath(const syd::RoiMaskImage & mask)
{
  if (mask.image == NULL) {
    LOG(FATAL) << "Error could not use GetDefaultRoiMaskImageRelativePath with image in the mask. Mask: " << mask;
  }
  std::string p = db_->GetRelativeFolder(*mask.image->patient)+PATH_SEPARATOR+"roi"+PATH_SEPARATOR;
  return p;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilder::UpdateDicom(syd::Image & image, const syd::DicomSerie & dicomserie)
{
  image.dicoms.push_back(std::make_shared<syd::DicomSerie>(dicomserie));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilder::UpdateFile(syd::Image & image,
                                   std::string relativepath,
                                   std::string filename)
{
  std::string extension = GetExtension(filename);
  image.type = extension;
  image.files.clear();
  syd::File f;
  f.filename = filename;
  f.path = relativepath;
  db_->Insert(f);
  image.files.push_back(std::make_shared<syd::File>(f));
  if (extension == "mhd") {
    syd::Replace(filename, ".mhd", ".raw");
    syd::File raw;
    raw.filename = filename;
    raw.path = relativepath;
    db_->Insert(raw);
    image.files.push_back(std::make_shared<syd::File>(raw));
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image syd::ImageBuilder::InsertEmptyImage(const syd::Tag & tag,
                                               const syd::Patient & patient)
{
  syd::Image image;
  image.patient = std::make_shared<syd::Patient>(patient);
  image.tag = std::make_shared<syd::Tag>(tag);
  db_->Insert(image);
  return image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage syd::ImageBuilder::InsertRoiMaskImage(const syd::Tag & tag,
                                                        const syd::DicomSerie & dicom,
                                                        const syd::RoiType & roitype,
                                                        const std::string & filename)
{
  // Create a mask
  syd::RoiMaskImage mask;
  mask.roitype = std::make_shared<syd::RoiType>(roitype);

  // Create the associated image
  syd::Image image = InsertEmptyImage(tag, *dicom.patient);

  // Net to set the image of the mask and insert it before GetDefaultRoiMaskImageRelativePath
  mask.image = std::make_shared<syd::Image>(image);
  db_->Insert(mask);

  // Update info
  UpdateDicom(image, dicom);
  UpdateFile(image, GetDefaultRoiMaskImageRelativePath(mask), GetDefaultRoiMaskImageFilename(mask));

  // Create image folder if needed
  std::string absolute_folder = db_->GetAbsoluteFolder(image);
  if (!syd::DirExists(absolute_folder)) syd::CreateDirectory(absolute_folder);

  // Read image
  typedef unsigned char PixelType;
  typedef itk::Image<PixelType,3> ImageType;
  ImageType::Pointer itk_image = syd::ReadImage<ImageType>(filename);

  // Update info
  UpdateImageInfo<PixelType>(image, itk_image, true); // true = update md5

  // Write image
  syd::WriteImage<ImageType>(itk_image, db_->GetAbsolutePath(image));

  // Insert into the db
  db_->Update(image);
  db_->Update(mask);
  return mask;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilder::CropImageLike(syd::Image & image, syd::Image & like, bool forceFlag)
{
  // Check
  if (!forceFlag) {
    std::string ref="";
    for(auto d:like.dicoms) {
      if (ref != "")
        if (ref != d->dicom_frame_of_reference_uid)
          LOG(WARNING) << "Image associated with several dicom_frame_of_reference_uid. " << like;
      ref = d->dicom_frame_of_reference_uid;
      DD(d->dicom_frame_of_reference_uid);
    }
    std::string ref2="";
    for(auto d:image.dicoms) {
      if (ref2 != "")
        if (ref2 != d->dicom_frame_of_reference_uid)
          LOG(WARNING) << "Image associated with several dicom_frame_of_reference_uid. " << like;
      ref2 = d->dicom_frame_of_reference_uid;
      DD(d->dicom_frame_of_reference_uid);
    }
    if (ref != ref2) {
      LOG(FATAL) << "Cannot crop: " << image << std::endl << "like: "
                 << like << std::endl
                 << "because they dont share the same dicom_frame_of_reference_uid (coordinate system).";
    }
  }

  // Crop the image and write it to disk
  if (image.pixel_type == "float") CropImageLike<float>(image, like);
  else if (image.pixel_type == "short") CropImageLike<short>(image, like);
  else if (image.pixel_type == "unsigned char") CropImageLike<unsigned char>(image, like);
  else {
    LOG(FATAL) << "Unknown pixel_type: " << image.pixel_type;
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilder::CropImageWithThreshold(syd::Image & image, double threshold)
{
  // Dispatch according to pixel type
  if (image.pixel_type == "float")  CropImageWithThreshold<float>(image, threshold);
  else if (image.pixel_type == "short")  CropImageWithThreshold<short>(image, threshold);
  else if (image.pixel_type == "unsigned char")  CropImageWithThreshold<unsigned char>(image, threshold);
  else {
    LOG(FATAL) << "Unknown pixel_type: " << image.pixel_type;
  }
}
// --------------------------------------------------------------------
