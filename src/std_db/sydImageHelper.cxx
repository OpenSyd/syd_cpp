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
#include "sydFileBuilder.h"

// --------------------------------------------------------------------
void syd::ImageHelper::
CopyAndSetMhdImage(syd::Image::pointer image, std::string filename)
{
  if (image->files.size() != 0)
    EXCEPTION("This image already contains files"
              << ". Remove them before using CopyAndSetMhdImage. Image is : "
              << image);
  InitializeEmptyMHDFiles(image);
  DD(image->GetAbsolutePath());
  CopyMHDImage(filename, image->GetAbsolutePath());
  DD("TODO : set the image param from header");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageHelper::
CopyInformation(syd::Image::pointer image, const syd::Image::pointer like)
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
void syd::ImageHelper::
InitializeEmptyMHDFiles(syd::Image::pointer image)
{
  image->type = "mhd";
  auto db = image->GetDatabase<syd::StandardDatabase>();
  syd::FileBuilder fb(db); // FIXME will be changed
  syd::File::pointer file_mhd = fb.NewFile(".mhd");
  syd::File::pointer file_raw = fb.NewFile();
  std::string f = file_mhd->filename;
  syd::Replace(f, ".mhd", ".raw");
  fb.RenameFile(file_raw, file_raw->path, f);
  image->files.push_back(file_mhd);
  image->files.push_back(file_raw);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ImageHelper::
SetPixelUnit(syd::Image::pointer image, std::string pixel_unit)
{
  auto db = image->GetDatabase<syd::StandardDatabase>();
}
// --------------------------------------------------------------------
