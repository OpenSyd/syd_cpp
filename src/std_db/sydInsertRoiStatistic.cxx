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
#include "sydInsertRoiStatistic_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
//#include "sydRoiStatisticBuilder.h" // FIXME
#include "sydCommonGengetopt.h"

#include <itkLabelStatisticsImageFilter.h>

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydInsertRoiStatistic, 2);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Read<syd::StandardDatabase>(args_info.db_arg);

  // Get the Image
  syd::IdType id = atoi(args_info.inputs[1]);
  syd::Image::pointer image;
  db->QueryOne(image, id);

  // Get the RoiMaskImage
  std::string roiname = args_info.inputs[0]; // FIXME or by id
  syd::RoiMaskImage::pointer roimask;
  //  db->FindRoiMaskImage(roimask, image, roi); // FIXME consider a single function for all that part
  syd::RoiType::pointer roitype = db->FindRoiType(roiname);
  typedef odb::query<syd::RoiMaskImage> Q;
  Q q = Q::roitype == roitype->id and Q::frame_of_reference_uid == image->frame_of_reference_uid;
  syd::RoiMaskImage::vector roimasks;
  db->Query(roimasks, q);
  if (roimasks.size() == 0) {
    LOG(FATAL) << "Cannot find a roimask of type '" << roitype->name
               << "' with same frame_of_reference_uid for the image: " << image;
  }
  if (roimasks.size() > 1) {
    LOG(FATAL) << "Several roimask exist with type '" << roitype->name
               << "' and same frame_of_reference_uid for the image: " << image;
  }
  roimask = roimasks[0];

  // log
  LOG(2) << "Input file: " << db->GetAbsolutePath(image);
  LOG(2) << "Input mask: " << db->GetAbsolutePath(roimask);

  // Get the itk images
  typedef float PixelType; // whatever the image
  typedef uchar MaskPixelType;
  typedef itk::Image<PixelType,3> ImageType;
  typedef itk::Image<MaskPixelType,3> MaskImageType;
  ImageType::Pointer itk_image = syd::ReadImage<ImageType>(db->GetAbsolutePath(image));
  MaskImageType::Pointer itk_mask = syd::ReadImage<MaskImageType>(db->GetAbsolutePath(roimask));

  // FIXME what about the number of value in the mask ?
  // should be in RoiMaskImage ?
  // int nb_pixels;
  // double volume;

  // If image is a ct, mean = mean HU, could be converted into density ? --> to include into RoiMaskImage ?
  // sydUpdateRoiMaskImage with ct ? bof.
  // no keep as a RoiStatistic special

  // FIXME resampling mask <-> image ? ; start with mask = resample like (image); syd::ResampleAndCropImageLike
  // (FIXME what about count !? warning) if resample image
  itk_mask = syd::ResampleAndCropImageLike<MaskImageType>(itk_mask, itk_image, 0, 0);

  // Statistics
  typedef itk::LabelStatisticsImageFilter<ImageType, MaskImageType> FilterType;
  typename FilterType::Pointer filter=FilterType::New();
  filter->SetInput(itk_image);
  filter->SetLabelInput(itk_mask);
  filter->Update();
  double mean = filter->GetMean(1);
  double std = filter->GetSigma(1);
  double n = filter->GetCount(1);
  double min = filter->GetMinimum(1);
  double max = filter->GetMaximum(1);
  double sum = filter->GetSum(1);

  // Create a RoiStatistic
  syd::RoiStatistic::pointer stat;
  db->New(stat);
  stat->image = image;
  stat->mask = roimask;
  stat->mean = mean;
  stat->std_dev = std;
  stat->n = n;
  stat->min = min;
  stat->max = max;
  stat->sum = sum;

  db->Insert(stat);
  LOG(1) << "Insert RoiStatistic: " << stat;

  // This is the end, my friend.
}
// --------------------------------------------------------------------
