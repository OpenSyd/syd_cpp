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

// --------------------------------------------------------------------
syd::RoiTimepoints::pointer syd::NewTimepoints(const syd::RoiStatistic::vector stats)
{
  if (stats.size() == 0) {
    EXCEPTION("Cannot create timepoints from empty vector of RoiStatistic");
  }
  syd::RoiTimepoints::pointer rtp;
  auto db = stats[0]->GetDatabase<syd::StandardDatabase>();
  auto patient = stats[0]->image->patient;
  auto mask = stats[0]->mask; // maybe nullptr
  auto injection = stats[0]->image->injection;

  db->New(rtp);
  rtp->patient = patient;
  rtp->injection = injection;

  // sort the roistat according to their times
  auto sorted_stats = stats;
  db->Sort<syd::RoiStatistic>(sorted_stats);
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
syd::FitTimepoints::pointer
syd::NewFitTimepoints(syd::Timepoints::pointer tp,
                      syd::TimeIntegratedActivityFitOptions & options)
{
  DDF();

  // Set or check lambda
  if (options.GetLambdaDecayConstantInHours() == 0.0)
    options.SetLambdaDecayConstantInHours(tp->injection->GetLambdaDecayConstantInHours());

  // Create FitTimepoints
  syd::FitTimepoints::pointer ft;
  auto db = tp->GetDatabase<syd::StandardDatabase>();
  db->New(ft);
  ft->timepoints = tp;
  ft->SetFromOptions(options);

  // Perform fit
  syd::TimeIntegratedActivityFilter filter;
  auto tac = syd::GetTAC(ft->timepoints);
  filter.SetTAC(tac);
  filter.SetOptions(options);
  filter.Run();

  // Get results
  auto model = filter.GetFitModel();
  if (model == nullptr) {
    DD("Fit unsuccesful");
    ft->auc = 0.0;
    ft->r2 = 0.0;
    ft->model_name = "";
    ft->params.clear();
  }
  else {
    DD("fit ok");
    if (options.GetRestrictedFlag()) {
      auto working_tac = filter.GetWorkingTAC(); // restricted tac
      DD(working_tac);
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
  DD(ft);
  ft->iterations = filter.GetNbOfIterations();
  DD(ft);

  return ft;
}
// --------------------------------------------------------------------
