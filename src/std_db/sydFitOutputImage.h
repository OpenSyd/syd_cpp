
#include "sydIntegratedActivityImageBuilder.h"


namespace syd {

  class FitOutputImage {
  public:

    typedef float PixelType;
    typedef itk::Image<PixelType,3> ImageType;
    typedef ImageType::Pointer Pointer;
    typedef itk::ImageRegionIterator<ImageType> Iterator;
    typedef itk::Image<PixelType,4> Image4DType;

    Pointer image;
    Iterator iterator;
    std::string filename;

    FitOutputImage(Pointer input) {
      image = ImageType::New();
      image->CopyInformation(input);
      image->SetRegions(input->GetLargestPossibleRegion());
      image->Allocate();
      iterator = Iterator(image, image->GetLargestPossibleRegion());
    }

    virtual void Update(const syd::TimeActivityCurve & tac,
                        const syd::FitModelBase * model,
                        const ceres::Solver::Summary & summary) = 0;

  };


  class FitOutputImage_AUC: public FitOutputImage {
  public:
    FitOutputImage_AUC(Pointer input):FitOutputImage(input) { filename = "auc.mhd"; }
    virtual void Update(const syd::TimeActivityCurve & tac,
                        const syd::FitModelBase * model,
                        const ceres::Solver::Summary & summary) {
      double r = model->ComputeAUC(tac);
      iterator.Set(r);
    }
  };


  class FitOutputImage_R2: public FitOutputImage {
  public:
    FitOutputImage_R2(Pointer input):FitOutputImage(input) { filename = "r2.mhd"; }
    virtual void Update(const syd::TimeActivityCurve & tac,
                        const syd::FitModelBase * model,
                        const ceres::Solver::Summary & summary) {

      double r = model->ComputeR2(tac);
      iterator.Set(r);
    }
  };


 // class FitOutputImage_AUC: public FitOutputImage {
 //  public:
 //    FitOutputImage_AUC(Pointer input):FitOutputImage(input) { filename = "auc.mhd"; }
 //    virtual void Update(const syd::TimeActivityCurve & tac,
 //                        const syd::FitModelBase * model,
 //                        const ceres::Solver::Summary & summary) {
 //      double r = model->ComputeAUC();
 //      iterator.Set(r);
 //    }
 //  };



} // end namespace
