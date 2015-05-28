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
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image syd::ImageBuilder::CreateImageFromDicomSerie(const syd::DicomSerie & dicomserie)
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

  // Get patient
  syd::Patient patient = *dicomserie.patient;

  // Create image first to get the id
  syd::Image image;
  image.patient = std::make_shared<syd::Patient>(patient);
  image.tag = std::make_shared<syd::Tag>(tag_);
  image.dicoms.push_back(std::make_shared<syd::DicomSerie>(dicomserie));
  image.type = "mhd";
  db_->Insert(image);

  // Define filename, path
  std::string path = db_->GetAbsoluteFolder(patient);
  std::ostringstream oss;
  oss << dicomserie.dicom_modality << "_" << image.id << ".mhd";
  std::string filename = oss.str();
  std::string output = path+PATH_SEPARATOR+filename;

  // Convert dicom, write to disk
  std::string md5;
  try {
    if (dicom_filenames.size() == 1) { // probably OT, NM image
      typedef float PixelType;
      typedef itk::Image<PixelType, 3> ImageType;
      ImageType::Pointer itk_image = syd::ReadDicomFromSingleFile(dicom_filenames[0]);
      syd::WriteImage<ImageType>(itk_image, output);
      UpdateImageInfo<ImageType>(image, itk_image);
      md5 = ComputeImageMD5<ImageType>(itk_image);
    }
    if (dicomserie.dicom_modality == "CT") {
      typedef short PixelType;
      typedef itk::Image<PixelType, 3> ImageType;
      std::string folder = dicom_filenames[0];
      syd::Replace(folder, GetFilenameFromPath(folder), "");
      ImageType::Pointer itk_image = syd::ReadDicomSerieFromFolder(folder, dicomserie.dicom_series_uid);
      syd::WriteImage<ImageType>(itk_image, output);
      UpdateImageInfo<ImageType>(image, itk_image);
      md5 = ComputeImageMD5<ImageType>(itk_image);
    }

    // Create File and Image object
    syd::File mhd;
    mhd.filename = filename;
    mhd.path = db_->GetRelativeFolder(patient);
    syd::Replace(filename, ".mhd", ".raw");
    syd::File raw;
    raw.filename = filename;
    raw.path = db_->GetRelativeFolder(patient);
    raw.md5 = md5;
    db_->Insert(mhd);
    db_->Insert(raw);
    image.files.push_back(std::make_shared<syd::File>(mhd));
    image.files.push_back(std::make_shared<syd::File>(raw)); // must be after file insertion
    db_->Update(image);
  } catch (syd::Exception & e) {
    db_->Delete(image);
    EXCEPTION("Error during CreateImageFromDicomSerie: " << e.what() << " (I remove the image " << image << ")");
  }

  return image;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageBuilder::CreateImagesInTimepoint(syd::Timepoint & timepoint)
{
  DD("CreateImagesInTimepoint");
  DD(timepoint);

  // loop create all images
  for(auto d:timepoint.dicoms) {
    syd::Image image = CreateImageFromDicomSerie(*d);
    DD(image);
    // Associate image with timepoint
    timepoint.images.push_back(std::make_shared<syd::Image>(image));
  }

  DD(timepoint);
  db_->Update(timepoint);
  DD(timepoint);
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
