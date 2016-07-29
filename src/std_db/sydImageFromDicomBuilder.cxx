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
#include "sydImageFromDicomBuilder.h"
#include "sydImageUtils.h"

// --------------------------------------------------------------------
syd::ImageFromDicomBuilder::ImageFromDicomBuilder()
{
  dicom_ = NULL;
  image_ = NULL;
  flipAxeIfNegativeFlag_ = true;
}
// --------------------------------------------------------------------

// --------------------------------------------------------------------
void syd::ImageFromDicomBuilder::Update()
{
  if (dicom_ == NULL) {
    EXCEPTION("Use SetInputDicomSerie first.");
  }

  DD("ImageFromDicomBuilder::Update");
  DD(dicom_);

  auto db = dicom_->GetDatabase();
  db->New(image_);
  image_->patient = dicom_->patient;
  image_->modality = dicom_->dicom_modality;
  DD(image_);

  // Get dicom associated files or folder
  //  syd::DicomFile::vector dicom_files_;
  odb::query<syd::DicomFile> q =
    odb::query<syd::DicomFile>::dicom_serie->id == dicom_->id;
  db->Query(dicom_files_, q);
  if (dicom_files_.size() == 0) {
    EXCEPTION("Error not DicomFile associated with this DicomSerie: " << dicom_);
  }
  std::vector<std::string> dicom_filenames;
  for(auto f:dicom_files_)
    dicom_filenames.push_back(f->GetAbsolutePath());
  DD(dicom_files_.size());

  // read dicom header with itk header (first file only)
  header_ = syd::ReadImageHeader(dicom_files_[0]->GetAbsolutePath());
  std::string pixel_type =
    itk::ImageIOBase::GetComponentTypeAsString(header_->GetComponentType());
  DD(pixel_type);

  // POLICY to switch short float ?
  // 1) user option
  // 2) try to guess ? CT = short, other = float ?
  if (image_->modality != "CT") {
    pixel_type = "float"; // force to float (even if short ?)
  }
  DD(pixel_type);

  // switch according to pixel type short, float other
  if (pixel_type == "short") UpdateWithPixelType<short>();
  else {
    if (pixel_type == "double") UpdateWithPixelType<double>();
    else UpdateWithPixelType<float>();
  }

  // =========> no need template
  // try to guess pixel_unit ?
  DD("try to guess pixel_unit ?");

  /*
    return image;
    syd::Image::pointer image = NewMHDImage(dicom);

    try {
    if (dicom->dicom_modality == "CT") {
    typedef short PixelType;
    typedef itk::Image<PixelType,3> ImageType;
    LOG(4) << "Read dicom (short)";
    ImageType::Pointer itk_image = db_->ReadImage<PixelType>(dicom, true);
    LOG(4) << "Update information";
    syd::PixelValueUnit::pointer unit = db_->FindOrInsertUnit("HU", "Hounsfield Units");
    image->pixel_unit = unit;
    if (dicom->dicom_pixel_scale != 1.0) {
    LOG(2) << "Pixel Scale = " << dicom->dicom_pixel_scale << ", so I scale the image.";
    syd::ScaleImage<ImageType>(itk_image, dicom->dicom_pixel_scale);
    }
    SetImage<PixelType>(image, itk_image);
    }
    else {
    typedef float PixelType;
    typedef itk::Image<PixelType,3> ImageType;
    LOG(4) << "Read dicom (float)";
    ImageType::Pointer itk_image = db_->ReadImage<PixelType>(dicom, true);
    LOG(4) << "Update information";
    syd::PixelValueUnit::pointer unit = db_->FindOrInsertUnit("counts", "Number of counts");
    image->pixel_unit = unit;
    // Scale if needed
    if (dicom->dicom_pixel_scale != 1.0) {
    LOG(2) << "Pixel Scale = " << dicom->dicom_pixel_scale << ", so I scale the image.";
    syd::ScaleImage<ImageType>(itk_image, dicom->dicom_pixel_scale);
    }
    SetImage<PixelType>(image, itk_image);
    }

    } catch (syd::Exception & e) {
    EXCEPTION("Error during InsertImage: " << e.what());
    }

    // return the image
    return image;
  */
}
// --------------------------------------------------------------------
