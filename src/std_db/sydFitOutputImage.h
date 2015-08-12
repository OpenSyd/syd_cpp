
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
      double r = model->ComputeAUC();
      iterator.Set(r);
    }
  };


  class FitOutputImage_R2: public FitOutputImage {
  public:
    FitOutputImage_R2(Pointer input):FitOutputImage(input) { filename = "r2.mhd"; }
    virtual void Update(const syd::TimeActivityCurve & tac,
                        const syd::FitModelBase * model,
                        const ceres::Solver::Summary & summary) {

      // R² = 1 - (SS_res/SS_tot)
      // SS_res = residual sum of squares
      // SS_tot = total sum of squares (prop to var)

      double mean = 0.0;
      for(auto i=0; i<tac.size(); i++) mean += tac.GetValue(i);
      mean = mean / (double)tac.size();

      double SS_res = summary.final_cost; // auto squared by ceres
      double SS_tot = 0.0;
      for(auto i=0; i<tac.size(); i++) {
        SS_tot += pow(tac.GetValue(i)-mean, 2);
      }
      double R2 = 1.0 - (SS_res/SS_tot);
      iterator.Set(R2);
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
