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
#include "sydTimepointsHelper.h"
#include "sydStandardDatabase.h"
#include "sydTimeIntegratedActivityFilter.h"
#include "sydImageHelper.h"

// --------------------------------------------------------------------
syd::RoiTimepoints::vector syd::FindRoiTimepoints(const syd::RoiStatistic::vector stats)
{
  syd::RoiTimepoints::vector rtp;
  if (stats.size() == 0) {
    EXCEPTION("Cannot FindRoiTimepoints with empty stats vector");
  }
  auto db = stats[0]->GetDatabase();
  db->Query(rtp);
  syd::RoiTimepoints::vector tp;
  for(auto r:rtp) {
    if (std::equal(r->roi_statistics.begin(), r->roi_statistics.end(), stats.begin()))
      tp.push_back(r);
  }
  return tp;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitTimepoints::vector syd::FindFitTimepoints(const syd::Timepoints::pointer tp,
                                                  const syd::TimeIntegratedActivityFitOptions & options)
{
  syd::FitTimepoints::vector ftp;
  auto db = tp->GetDatabase();
  typedef odb::query<syd::FitTimepoints> Q;
  Q q =
    Q::timepoints == tp->id and
    Q::r2_min == options.GetR2MinThreshold() and
    Q::max_iteration == options.GetMaxNumIterations() and
    Q::restricted_tac == options.GetRestrictedFlag() and
    Q::akaike_criterion == options.GetAkaikeCriterion();
  db->Query(ftp, q);
  syd::FitTimepoints::vector fftp;
  for(auto f:ftp) {
    if (f->GetModelsName() == options.GetModelsName()) fftp.push_back(f);
  }
  return fftp;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiTimepoints::pointer
syd::NewRoiTimepoints(const syd::RoiStatistic::vector stats)
{
  if (stats.size() == 0) {
    EXCEPTION("Cannot create timepoints from empty vector of RoiStatistic");
  }
  auto db = stats[0]->GetDatabase();
  auto patient = stats[0]->image->patient;
  auto mask = stats[0]->mask; // maybe nullptr
  auto injection = stats[0]->image->injection;

  auto rtp = db->New<syd::RoiTimepoints>();
  rtp->patient = patient;
  rtp->injection = injection;

  // sort the roistat according to their times
  auto sorted_stats = stats;
  db->Sort<syd::RoiStatistic>(sorted_stats, "image.acquisition_date");
  rtp->roi_statistics = sorted_stats;

  for(auto stat:sorted_stats) {
    if (stat->image->patient->id != patient->id) {
      EXCEPTION("The RoiStatistic do not have the same patient");
    }
    if (stat->image->injection->id != injection->id) {
      EXCEPTION("The RoiStatistic do not have the same injection");
    }
    if ((mask == nullptr and stat->mask != mask) or
        (mask != nullptr and stat->mask == nullptr) or
        (mask->id != stat->mask->id)) {
      EXCEPTION("The RoiStatistic do not have the same mask");
    }
    rtp->times.push_back(stat->image->GetHoursFromInjection());
    rtp->values.push_back(stat->mean);
  }
  return rtp;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/// Create a TAC from a tp
syd::TimeActivityCurve::pointer syd::GetTAC(const syd::Timepoints::pointer tp)
{
  auto tac = syd::TimeActivityCurve::New();
  GetTAC(tp, tac);
  return tac;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::GetTAC(const syd::Timepoints::pointer tp, syd::TimeActivityCurve::pointer tac)
{
  tac->Clear();
  for(auto i=0; i<tp->times.size(); i++)
    tac->AddValue(tp->times[i], tp->values[i]);//, tp->std_deviations[i]); FIXME
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::CopyTimepoints(const syd::Timepoints::pointer from, syd::Timepoints::pointer to)
{
  DDF();
  DD("TODO");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::ComputeFitTimepoints(syd::FitTimepoints::pointer ft)
{
  // Perform fit
  syd::TimeIntegratedActivityFilter filter;
  auto tac = syd::GetTAC(ft->timepoints);
  auto options = ft->GetOptions();
  filter.SetTAC(tac);
  filter.SetOptions(options);
  filter.Run();

  // Get results
  auto model = filter.GetFitModel();
  if (model == nullptr) {
    ft->auc = 0.0;
    ft->r2 = 0.0;
    ft->model_name = "";
    ft->params.clear();
  }
  else {
    if (options.GetRestrictedFlag()) {
      auto working_tac = filter.GetWorkingTAC(); // restricted tac
      ft->first_index = tac->GetSize()-working_tac->GetSize();
      ft->auc = model->ComputeAUC(working_tac, ft->first_index);
      ft->r2  = model->ComputeR2(working_tac);
    }
    else {
      ft->first_index = 0;
      ft->auc = model->Integrate();
      ft->r2  = model->ComputeR2(tac);
    }
    ft->model_name = model->GetName();
    ft->params = model->GetParameters();
  }
  ft->iterations = filter.GetNbOfIterations();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::FitTimepoints::pointer
syd::NewFitTimepoints(const syd::Timepoints::pointer tp,
                      syd::TimeIntegratedActivityFitOptions & options)
{
  // Set or check lambda
  if (options.GetLambdaDecayConstantInHours() == 0.0)
    options.SetLambdaDecayConstantInHours(tp->injection->GetLambdaDecayConstantInHours());

  // Create FitTimepoints
  auto db = tp->GetDatabase();
  auto ft = db->New<syd::FitTimepoints>();
  ft->timepoints = tp;
  ft->SetFromOptions(options);

  // Compute
  ComputeFitTimepoints(ft);
  return ft;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Timepoints::pointer
syd::NewTimepointsAtPixel(const syd::Image::vector & images,
                          const std::vector<double> & pixel)
{
  if (images.size() == 0) return nullptr;

  // Get the db pointer
  auto db = images[0]->GetDatabase<syd::StandardDatabase>();

  // Check images ? (same patient etc ?) FIXME LATER
  auto patient = images[0]->patient;
  auto injection = images[0]->injection; // could be null
  if (injection == nullptr) return nullptr;

  // Create a tp
  auto tp = db->New<syd::Timepoints>();
  tp->patient = patient;
  tp->injection = injection;

  // Get the times (no values yet)
  tp->times = syd::GetTimesFromInjection(images);

  // Read all itk images
  typedef float PixelType;
  typedef itk::Image<PixelType, 3> ImageType;
  std::vector<ImageType::Pointer> itk_images;
  for(auto image:images) {
    auto itk_image = syd::ReadImage<ImageType>(image->GetAbsolutePath());
    itk_images.push_back(itk_image);
  }

  // Get pixel index
  ImageType::IndexType index;
  ImageType::PointType point;
  point[0] = pixel[0];
  point[1] = pixel[1];
  point[2] = pixel[2];
  itk_images[0]->TransformPhysicalPointToIndex(point, index);

  // Set tac values
  auto n = itk_images.size();
  tp->values.resize(n);
  tp->std_deviations.resize(n);
  for(auto i=0; i<n; i++) {
    tp->values[i] = itk_images[i]->GetPixel(index);
    tp->std_deviations[i] = 0.0;
  }

  // Return
  return tp;
}
// --------------------------------------------------------------------
