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
void syd::ImageHelper::
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
}
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
