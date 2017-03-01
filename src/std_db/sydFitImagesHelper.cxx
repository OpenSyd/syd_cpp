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
#include "sydFitImagesHelper.h"
#include "sydTimepointsHelper.h"

// --------------------------------------------------------------------
syd::Image::pointer syd::NewOutputFitImages(const syd::FitImages::pointer tia,
                                            const std::string fit_output_name)
{
  DDF();
  DD(tia);
  DD(fit_output_name);

  auto params = tia->GetOutput("fit_p");
  auto success = tia->GetOutput("fit_1");
  auto models = tia->GetOutput("fit_mo");
  DD(params);
  DD(success);
  DD(models);

  LOG(FATAL) << "not implemented";
  return nullptr;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitTimepoints::pointer
syd::NewFitTimepointsAtPixel(const syd::FitImages::pointer tia,
                             const std::vector<double> & pixel)
{
  DDF();
  DD(tia);
  DDS(pixel);

  auto db = tia->GetDatabase<syd::StandardDatabase>();

  auto ftp = db->New<syd::FitTimepoints>();
  DD(ftp);

  auto tp = syd::NewTimepoints(tia->images, pixel);
  DD(tp);
  ftp->timepoints = tp;

  // Fit options from tia
  auto options = tia->GetOptions();
  DD(options);
  ftp->SetFromOptions(options);
  DD(ftp);

  // Type of image
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;

  // Prepare pixel coord in index
  ImageType::IndexType index;
  ImageType::PointType point;
  point[0] = pixel[0];
  point[1] = pixel[1];
  point[2] = pixel[2];

  // Find success
  DD("success");
  auto success = tia->GetOutput("fit_1");
  auto itk_success = syd::ReadImage<ImageType>(success->GetAbsolutePath());
  itk_success->TransformPhysicalPointToIndex(point, index);
  DD(index);
  auto value = itk_success->GetPixel(index);
  DD(value);
  if (value == 0) { // fit was not successful, do nothing
    ftp->iterations = 0;
    return ftp;
  }

  // Find model
  DD("model");
  auto model = tia->GetOutput("fit_mo");
  auto itk_model = syd::ReadImage<ImageType>(model->GetAbsolutePath());
  itk_model->TransformPhysicalPointToIndex(point, index);
  DD(index);
  value = itk_model->GetPixel(index)-1; // -1 because model=0 means no model
  DD(value); // ()should not be -1)
  DD(ftp->model_names.size());
  ftp->model_name = ftp->model_names[value];
  DD(ftp);

  // Find first_index
  // FIXME later ? not needed ?

  // Find iterations
  DD("iterations");
  auto iterations = tia->GetOutput("fit_it");
  auto itk_iterations = syd::ReadImage<ImageType>(iterations->GetAbsolutePath());
  itk_iterations->TransformPhysicalPointToIndex(point, index);
  DD(index);
  value = itk_iterations->GetPixel(index);
  DD(value); // ()should not be -1)
  ftp->iterations = value;
  DD(ftp);

  // Params
  typedef itk::Image<PixelType, 4> ParamImageType;
  ParamImageType::IndexType pindex;
  ParamImageType::PointType ppoint;
  ppoint[0] = pixel[0];
  ppoint[1] = pixel[1];
  ppoint[2] = pixel[2];
  auto params = tia->GetOutput("fit_p");
  auto itk_params = syd::ReadImage<ParamImageType>(params->GetAbsolutePath());
  auto n = itk_params->GetLargestPossibleRegion().GetSize()[3];
  DD(n);
  ftp->params.resize(n);
  for(auto i=0; i<n; i++) {
    ppoint[3] = i;
    itk_params->TransformPhysicalPointToIndex(ppoint, pindex);
    DD(pindex);
    value = itk_params->GetPixel(pindex);
    DD(value);
    ftp->params[i] = value;
  }
  DD(ftp);

  //  need to build a FitTimepoints ? (in helper)
  // Need 3) first_index from image
  // Need 3) params from image
  // Need 3) auc r2 not needed

  return ftp;
}
// --------------------------------------------------------------------

