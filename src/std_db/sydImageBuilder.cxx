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
  tag_.label = "unamed_tag";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image syd::ImageBuilder::InsertImageFromDicomSerie(const syd::DicomSerie & dicomserie)
{
  if (tag_.label == "unamed_tag") {
    EXCEPTION("Error in ImageBuilder, use SetImageTag before");
  }

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

  // Get patient
  syd::Patient patient = *dicomserie.patient;

  // Create image first to get the id
  syd::Image image = InitializeNewMHDImage(patient, tag_, dicomserie);
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
    EXCEPTION("Error during CreateImageFromDicomSerie: " << e.what() << " (I remove the image " << image << ")");
  }

  return image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilder::InsertImagesFromTimepoint(syd::Timepoint & timepoint)
{

  DD("InsertImagesFromTimepoint TODO");
  exit(0);

  // loop create all images
  for(auto d:timepoint.dicoms) {
    // Create new image
    syd::Image image = InsertImageFromDicomSerie(*d);
    // Associate image with timepoint
    timepoint.images.push_back(std::make_shared<syd::Image>(image));
  }
  db_->Update(timepoint);
  //  LOG(1) << "Timpoint (" << timepoint << ") updated with " << timepoint.dicoms.size() << " images."; // FIXME not here
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
syd::Image syd::ImageBuilder::StitchDicomSerie(const syd::DicomSerie & a,
                                               const syd::DicomSerie & b)
{
  if (tag_.label == "unamed_tag") {
    EXCEPTION("Error in ImageBuilder, use SetImageTag before");
  }

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
  syd::Image image = InitializeNewMHDImage(*a.patient, tag_, a);

  // Also link the second dicom
  image.dicoms.push_back(std::make_shared<syd::DicomSerie>(b));

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
std::string syd::ImageBuilder::GetDefaultImageFilename(const syd::Image & image,
                                                       const syd::DicomSerie & dicomserie)
{
  std::ostringstream oss;
  oss << dicomserie.dicom_modality << "_" << image.id << ".mhd";
  std::string filename = oss.str();
  return filename;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image syd::ImageBuilder::InitializeNewMHDImage(const syd::Patient & patient,
                                                    const syd::Tag & tag,
                                                    const syd::DicomSerie & dicomserie)
{
  // Create image first to get the id
  syd::Image image;
  image.patient = std::make_shared<syd::Patient>(patient);
  image.tag = std::make_shared<syd::Tag>(tag_);
  image.dicoms.push_back(std::make_shared<syd::DicomSerie>(dicomserie));
  image.type = "mhd";
  db_->Insert(image);

  // Define filename and create the associated Files
  std::string filename = GetDefaultImageFilename(image, dicomserie);

  // Create files
  std::vector<syd::File> files(2); // .mhd and .raw
  files[0].filename = filename;
  files[0].path = db_->GetRelativeFolder(*image.patient);
  syd::Replace(filename, ".mhd", ".raw");
  files[1].filename = filename;
  files[1].path = db_->GetRelativeFolder(patient);
  db_->Insert(files[0]);
  db_->Insert(files[1]);

  // Set the Files
  image.files.clear();
  image.files.push_back(std::make_shared<syd::File>(files[0]));
  image.files.push_back(std::make_shared<syd::File>(files[1])); // must be after file insertion

  // (image is not updated in the db)
  return image;
}
// --------------------------------------------------------------------
