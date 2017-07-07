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
#include "sydTimeIntegratedActivityFilter.h"
#include "sydStandardDatabase.h"

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
std::string syd::GetModelNameFromId(int value)
{
  // Single time initialization (FIXME: later do a static global map ?)
  static syd::FitModelBase::vector models;
  static bool first_time = true;
  if (first_time) {
    models.push_back(std::make_shared<syd::FitModel_f1>());
    models.push_back(std::make_shared<syd::FitModel_f2>());
    models.push_back(std::make_shared<syd::FitModel_f3>());
    models.push_back(std::make_shared<syd::FitModel_f4a>());
    models.push_back(std::make_shared<syd::FitModel_f4b>());
    models.push_back(std::make_shared<syd::FitModel_f4c>());
    models.push_back(std::make_shared<syd::FitModel_f4>());
    first_time = false;
  }
  for(auto m:models)
    if (m->GetId() == value) return m->GetName();
  return "model_id_not_found";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitTimepoints::pointer
syd::NewFitTimepointsAtPixel(const syd::FitImages::pointer tia,
                             const std::vector<double> & pixel)
{
  auto db = tia->GetDatabase<syd::StandardDatabase>();
  auto ftp = db->New<syd::FitTimepoints>();
  auto tp = syd::NewTimepointsAtPixel(tia->images, pixel);
  ftp->timepoints = tp;

  // Fit options from tia
  auto options = tia->GetOptions();
  ftp->SetFromOptions(options);

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
  auto success = tia->GetOutput("fit_1");
  auto itk_success = syd::ReadImage<ImageType>(success->GetAbsolutePath());
  itk_success->TransformPhysicalPointToIndex(point, index);
  auto value = itk_success->GetPixel(index);
  if (value == 0) { // fit was not successful, do nothing
    ftp->iterations = 0;
    return ftp;
  }

  // Find model
  auto model = tia->GetOutput("fit_mo");
  auto itk_model = syd::ReadImage<ImageType>(model->GetAbsolutePath());
  itk_model->TransformPhysicalPointToIndex(point, index);
  value = itk_model->GetPixel(index); // this is the model id
  ftp->model_name = syd::GetModelNameFromId(value);

  // Find first_index
  // FIXME later ? not needed ?

  // Find iterations
  auto iterations = tia->GetOutput("fit_it");
  auto itk_iterations = syd::ReadImage<ImageType>(iterations->GetAbsolutePath());
  itk_iterations->TransformPhysicalPointToIndex(point, index);
  value = itk_iterations->GetPixel(index);
  ftp->iterations = value;

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
  ftp->params.resize(n);
  for(auto i=0; i<n; i++) {
    ppoint[3] = i;
    itk_params->TransformPhysicalPointToIndex(ppoint, pindex);
    value = itk_params->GetPixel(pindex);
    ftp->params[i] = value;
  }

  // end
  return ftp;
}
// --------------------------------------------------------------------

