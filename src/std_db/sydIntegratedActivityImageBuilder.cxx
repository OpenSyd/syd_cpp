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
#include "sydIntegratedActivityImageBuilder.h"
#include "sydImageUtils.h"

// itk
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIterator.h>

// --------------------------------------------------------------------
syd::IntegratedActivityImageBuilder::IntegratedActivityImageBuilder(syd::StandardDatabase * db)
{
  db_ = db;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::SetInput(syd::Image::vector & images)
{
  // Check sizes, order by date
  images_ = images;

  // Sort by acquisition date
  std::sort(begin(images_), end(images_),
            [images](syd::Image::pointer a, syd::Image::pointer b) {
              if (a->dicoms.size() == 0) return true;
              if (b->dicoms.size() == 0) return false;
              return a->dicoms[0]->acquisition_date < b->dicoms[0]->acquisition_date;
            });


}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::pointer syd::IntegratedActivityImageBuilder::GetOutput() const
{
  return output_;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::SaveDebugPixel(const std::string & filename) const
{
  syd::PrintTable ta;
  ta.AddColumn("#time", 10);
  for(auto index:debug_pixels) ta.AddColumn(syd::ToString(index), 5);
  ta.Init();
  for(auto t=0; t<current_tac_.size(); t++) {
    ta << current_tac_.GetTime(t);
    for(auto tac:debug_tac) { DD(tac); ta << tac.GetValue(t); }
    ta.Endl();
  }
  std::ofstream os(filename);
  ta.Print(os);
  os.close();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::AddDebugPixel(int x, int y, int z)
{
  if (images_.size() == 0) {
    LOG(FATAL) << "Only use AddDebugPixel after SetInput";
  }
  auto size = images_[0]->size;
  int index = z*size[0]*size[1] + y*size[0] + x;
  debug_pixels.push_back(index);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::IntegratedActivityImageBuilder::CreateIntegratedActivityImage()
{

  // FIXME separate itk only

  // alloc debug
  debug_tac.resize(debug_pixels.size());

  // typedef
  typedef float PixelType;
  typedef itk::Image<PixelType,3> ImageType;

  // Load itk images
  std::vector<ImageType::Pointer> itk_images;
  for(auto image:images_) {
    auto im = syd::ReadImage<ImageType>(db_->GetAbsolutePath(image));
    itk_images.push_back(im);
  }

  // consider images_ OR create a 4D images ?
  typedef itk::Image<PixelType,4> Image4DType;
  Image4DType::Pointer tac_image = Image4DType::New();
  typename Image4DType::SizeType size;
  for(auto i=1; i<4; i++) size[i] = itk_images[0]->GetLargestPossibleRegion().GetSize()[i-1];
  size[0] = itk_images.size();
  typename Image4DType::RegionType region;
  region.SetSize(size);
  tac_image->SetRegions(region);
  typename Image4DType::SpacingType spacing;
  for(auto i=1; i<4; i++) spacing[i] = itk_images[0]->GetSpacing()[i-1];
  spacing[0] = 1.0;
  tac_image->SetSpacing(spacing);
  typename Image4DType::PointType origin;
  for(auto i=1; i<4; i++) origin[i] = itk_images[0]->GetOrigin()[i-1];
  origin[0] = 0.0;
  tac_image->SetOrigin(origin);
  tac_image->Allocate();

  typedef itk::ImageRegionIterator<ImageType> Iterator3D;
  typedef itk::ImageRegionIterator<Image4DType> Iterator4D;
  Iterator4D it(tac_image, tac_image->GetLargestPossibleRegion());
  std::vector<Iterator3D> iterators;
  for(auto image:itk_images) {
    iterators.push_back(Iterator3D(image, image->GetLargestPossibleRegion()));
  }
  for(auto & iter:iterators) iter.GoToBegin();

  DD("loop");
  for (it.GoToBegin(); !it.IsAtEnd(); ) {
    for(auto & iter:iterators) {
      it.Set(iter.Get());
      ++iter;
      ++it;
    }
  }

  //WriteImage<Image4DType>(tac_image, "4D.mhd"); hard to see
  DD("allocate");
  ImageType::Pointer itk_output = ImageType::New();
  itk_output->CopyInformation(itk_images[0]);
  itk_output->SetRegions(itk_images[0]->GetLargestPossibleRegion());
  itk_output->Allocate();
  Iterator3D out_iter(itk_output, itk_output->GetLargestPossibleRegion());

  // Calibration factor here ?

  // create output image both db+itk

  // create initial tac
  std::string starting_date = images_[0]->dicoms[0]->injection->date;
  for(auto image:images_) {
    double t = DateDifferenceInHours(image->dicoms[0]->acquisition_date, starting_date);
    current_tac_.AddValue(t, 0.0);
  }
  DD(current_tac_);

  // loop pixel ? list of iterators
  //  update values of the tac
  //  call tac integration
  //  check result
  //  set result to output image
  out_iter.GoToBegin();
  int index=0;
  for (it.GoToBegin(); !it.IsAtEnd(); ) {

    // Compute the tac
    for(auto i=0; i<itk_images.size(); i++) {
      current_tac_.SetValue(i, it.Get());
      ++it; // next value
    }

    // Integration
    double v = Integrate();

    // Save for debug
    auto iter = std::find(debug_pixels.begin(), debug_pixels.end(), index);
    if (iter != debug_pixels.end()) {
      int x = iter - debug_pixels.begin();
      DD(x);
      debug_tac[x] = current_tac_;
      //debug_model[x] =  fixme later
    }

    // Next
    out_iter.Set(v);
    ++out_iter;
    ++index;
  }

  // write result image
  WriteImage<ImageType>(itk_output, "debug.mhd");

  // update db (only once it is finished)

  DD("end");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::IntegratedActivityImageBuilder::Integrate()
{

  // First point
  double t1 = 0;
  double t2 = current_tac_.GetTime(0);
  double A1 = 0;
  double A2 = current_tac_.GetValue(0);
  double r = (t2-t1)*(A2+A1)/2.0;//A1*t1 - t1*t1*(A1-A2)/(t1-t2)/2.0;

  // other points
  for(auto i=1; i<current_tac_.size(); i++) {
    t1 = current_tac_.GetTime(i-1);
    t2 = current_tac_.GetTime(i);
    A1 = current_tac_.GetValue(i-1);
    A2 = current_tac_.GetValue(i);
    // double a = tac_->GetValue(i);
    //double b = tac_->GetValue(i+1);
    //    double d = tac_->GetTime(i+1)-tac_->GetTime(i);
    r = r + (t2-t1)*(A2+A1)/2.0;
  }

  // Last point ? mono expo fit with physical half life only (?)

  return r;
}
// --------------------------------------------------------------------
